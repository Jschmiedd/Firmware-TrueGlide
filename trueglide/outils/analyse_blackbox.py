#!/usr/bin/env python3
"""Analyse de journaux blackbox Betaflight / TrueGlide : rapport en français (Markdown).

Pour chaque vol : configuration (firmware, carte, protocole DShot, boucle PID), puis défauts constatés :
- télémétrie DShot figée (le contrôleur ne reçoit plus de trame valide d'un ESC : vitesse bloquée) ;
- moteur désynchronisé ou calé (commandé fort mais beaucoup plus lent que les autres), détecté avec la logique
  du firmware TrueGlide (src/main/flight/motor_health.c, compilée telle quelle) ;
- saturation des moteurs, bruit gyro, oscillation lente ;
- nombre de pôles (aimants) des moteurs mesuré, comparé à `motor_poles` : une erreur fausse le filtre RPM et le
  ralenti dynamique.

Usage : python3 trueglide/outils/analyse_blackbox.py journal.bbl [autres.bbl] [-o rapport.md]
        [--retour nom_retour_trueglide.json]
--retour écrit les mesures faites en vol pour le calculateur TALKTRUE FPV XRIG (format talktrue-trueglide-retour,
séparé de celui de XPro) : voir LISEZMOI.md, « Mesures de vol pour le calculateur ».
Dépendances : orangebox (pip install orangebox), numpy, gcc (pour la détection de désynchronisation).
"""
import argparse, datetime, json, os, sys
import numpy as np
from orangebox import Parser

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rejouer_desync as rj  # noqa: E402

PROTOCOLES = ['PWM', 'ONESHOT125', 'ONESHOT42', 'MULTISHOT', 'BRUSHED', 'DSHOT150', 'DSHOT300', 'DSHOT600', 'PROSHOT1000']


def charger(chemin, idx):
    try:
        p = Parser.load(chemin); p.set_log_index(idx)
    except (IndexError, KeyError, ValueError):
        return None   # en-tête de vol abîmé (coupure pendant l'écriture) : vol ignoré
    noms = p.field_names; n = len(noms)

    def num(v):
        try: return float(v)
        except (TypeError, ValueError): return np.nan
    lignes = []
    try:
        for fr in p.frames():
            if len(fr.data) == n:
                lignes.append([num(v) for v in fr.data])
    except (TypeError, ValueError, IndexError):
        pass   # journal tronqué (batterie débranchée en vol) : on garde ce qui a été lu
    A = np.array(lignes)
    if len(A) < 100:
        return None
    A = A[~np.isnan(A[:, 1])]
    if len(A) < 100:
        return None
    return p.headers, {k: i for i, k in enumerate(noms)}, A


def telemetrie_figee(e, cmd, actif):
    """% du temps moteur actif où l'eRPM reste identique >= 8 échantillons alors que la commande bouge de > 5 %."""
    res = []
    for i in range(e.shape[1]):
        fig = np.zeros(len(e), bool); k = 0
        while k < len(e):
            j = k
            while j + 1 < len(e) and e[j + 1, i] == e[k, i]:
                j += 1
            if j - k + 1 >= 8 and e[k, i] > 0 and np.ptp(cmd[k:j + 1, i]) > 0.05:
                fig[k:j + 1] = True
            k = j + 1
        res.append(100 * (fig & actif[:, i]).sum() / max(1, actif[:, i].sum()))
    return res


POLES_CANDIDATS = list(range(8, 50, 2))   # jusqu'à 48 : moteurs plats à flux axial, gros moteurs


def mesurer_poles(H, c, A):
    """Nombre de pôles mesuré : le balourd de chaque moteur fait vibrer le cadre à sa vitesse mécanique
    (eRPM / (pôles / 2) / 60 Hz). Sur des fenêtres de vol à vitesse stable, on cherche le nombre de pôles pour
    lequel le gyro brut montre le plus d'énergie à cette fréquence. Indépendant du réglage motor_poles (la
    télémétrie donne une vitesse électrique). Retourne {pôles: score moyen}, nombre de fenêtres ; ou None."""
    champs = rj.colonnes_erpm(list(c), H)
    if not champs or 'gyroUnfilt[0]' not in c:
        return None
    t = (A[:, c['time']] - A[0, c['time']]) * 1e-6
    dt = np.median(np.diff(t))
    e = A[:, [c[k] for k in champs]] * 100                      # eRPM
    g = A[:, [c[f'gyroUnfilt[{k}]'] for k in range(3)]]
    arme = (np.nan_to_num(A[:, c['flightModeFlags']]).astype(int) & 1) == 1
    N, pas = 512, 256                                         # ~0,5 s : résolution suffisante pour 48 pôles
    f = np.fft.rfftfreq(N, dt); fen = np.hanning(N)
    total = dict.fromkeys(POLES_CANDIDATS, 0.0); n = 0
    for s in range(0, len(A) - N, pas):
        sl = slice(s, s + N)
        if not arme[sl].all():
            continue
        em = e[sl].mean(axis=0)
        if em.min() < 2000 or (e[sl].std(axis=0) / em).max() > 0.05:   # vitesse trop basse ou qui varie
            continue
        sp = sum(np.abs(np.fft.rfft((g[sl, k] - g[sl, k].mean()) * fen)) ** 2 for k in range(3))
        ref = np.median(sp[(f > 30) & (f < f[-1] * 0.9)])
        if not ref > 0:
            continue
        for p in POLES_CANDIDATS:
            # fondamentale seulement : les harmoniques confondraient p et 2p
            v = [np.interp(fr, f, sp) / ref for fr in em / (p / 2) / 60 if fr < f[-1] * 0.95]
            if v:
                total[p] += np.log(np.mean(v))
        n += 1
    if n == 0:
        return None
    return {p: total[p] / n for p in POLES_CANDIDATS}, n


def conclure_poles(mesures):
    """Regroupe les mesures de plusieurs vols. Conclut seulement si assez de fenêtres et un net vainqueur.
    Retourne (pôles, exact) : exact = False quand le voisin à ±2 pôles reste proche (moteurs à beaucoup d'aimants)."""
    n = sum(m[1] for m in mesures)
    if n < 20:
        return None
    s = {p: sum(m[0][p] * m[1] for m in mesures) / n for p in POLES_CANDIDATS}
    ordre = sorted(s, key=s.get, reverse=True)
    marge = s[ordre[0]] - s[ordre[1]]
    if s[ordre[0]] < 3.0 or marge < 1.0:
        return None
    return ordre[0], marge >= 2.0


def analyser_vol(lib, chemin, idx):
    d = charger(chemin, idx)
    if d is None:
        return None
    H, c, A = d
    t = (A[:, c['time']] - A[0, c['time']]) * 1e-6
    dt = np.median(np.diff(t))
    lo, hi = H.get('motorOutput', [48, 2047])[:2]
    mot = A[:, [c[f'motor[{i}]'] for i in range(rj.nb_moteurs(c))]]
    cmd = np.clip((mot - lo) / (hi - lo), 0, 1)
    arme = (np.nan_to_num(A[:, c['flightModeFlags']]).astype(int) & 1) == 1
    r = {'idx': idx, 'duree': t[-1], 'H': H, 'defauts': [], 'mesures': {}}
    r['mesures']['saturation'] = 100 * (mot[arme] >= hi - 1).any(axis=1).mean() if arme.any() else 0
    if r['mesures']['saturation'] > 5:
        r['defauts'].append(f"moteurs saturés (100 %) pendant {r['mesures']['saturation']:.0f} % du vol : le PID demande plus que "
                            "le drone ne peut donner (moteur défaillant, hélice abîmée, crash ou réglage trop fort)")
    champs = rj.colonnes_erpm(list(c), H)
    if champs:
        e = A[:, [c[k] for k in champs]]
        fig = telemetrie_figee(e, cmd, arme[:, None] & (cmd > 0.08))
        r['mesures']['figee'] = fig
        for i, f in enumerate(fig):
            if f >= 1.0:
                r['defauts'].append(f'télémétrie DShot du moteur {i + 1} figée {f:.1f} % du temps (trames perdues : '
                                    'vérifier soudure du fil signal, câble ESC-FC, alimentation de l\'ESC)')
        if lib is not None:
            dd = rj.lire(chemin, idx)
            rel = rj.rejouer(lib, *dd, 40.0)
            r['relances'] = rel
            par = {}
            for tk, i, cc, rr in rel:
                par.setdefault(i, []).append((tk, cc, rr))
            for i, lst in sorted(par.items()):
                tk, cc, rr = lst[0]
                r['defauts'].append(f'moteur {i + 1} désynchronisé ou calé {len(lst)} fois (premier à {tk:.2f} s : commandé '
                                    f'{cc[i] * 100:.0f} %, {rr[i]:.0f} tr/min contre {np.delete(rr, i).min():.0f} à '
                                    f'{np.delete(rr, i).max():.0f} pour les autres)')
    mp = mesurer_poles(H, c, A)
    if mp:
        r['poles'] = mp
    vol = arme & (cmd.mean(axis=1) > 0.15)
    if vol.sum() * dt > 1.0:
        g = 'gyroUnfilt[0]' if 'gyroUnfilt[0]' in c else 'gyroADC[0]'
        gf = A[vol][:, [c['gyroADC[0]'], c['gyroADC[1]']]]
        gu = A[vol][:, [c[g], c[g.replace('[0]', '[1]')]]]
        r['mesures']['bruit_brut'] = float(np.std(np.diff(gu, axis=0)) / np.sqrt(2))
        r['mesures']['bruit_filtre'] = float(np.std(np.diff(gf, axis=0)) / np.sqrt(2))
        x = gf[:, 0] - gf[:, 0].mean(); n = 1 << int(np.log2(len(x)))
        sp = np.abs(np.fft.rfft(x[:n] * np.hanning(n))); f = np.fft.rfftfreq(n, dt)
        m = (f > 8) & (f < 100)
        pic = f[m][np.argmax(sp[m])]
        part = sp[m].max() ** 2 / max(1e-9, (sp[f > 8] ** 2).sum())
        r['mesures']['oscillation_hz'] = float(pic)
        if part > 0.05 and r['mesures']['bruit_filtre'] > 3:
            r['defauts'].append(f'oscillation lente vers {pic:.0f} Hz dans le gyro filtré (propwash ou P/D mal dosés)')
        r['mesures']['bruit_moteurs'] = float(np.std(np.diff(cmd[vol], axis=0)) * 100)
        if r['mesures']['bruit_moteurs'] > 2:
            r['defauts'].append(f"commande moteur bruitée ({r['mesures']['bruit_moteurs']:.1f} % par pas) : moteurs qui chauffent, "
                                'filtrage du D à renforcer')
    return r


# Stationnaire : fenêtres d'une seconde, armé, gaz stables, manches roulis / tangage / lacet au centre, drone stable
# et moteurs non saturés. Le stationnaire n'est retenu qu'avec au moins 3 s de telles fenêtres.
FENETRE_S = 1.0
STAT_GAZ_ECART = 15          # écart type des gaz (rcCommand, 1000 à 2000) : 1,5 %
STAT_MANCHE = 40             # |rcCommand| roulis / tangage / lacet (±500) : manches au centre
STAT_GYRO = 40.0             # écart type du gyro filtré roulis / tangage (°/s)
STAT_MIN_S = 3.0


def stationnaire(H, c, A, poles):
    """Mesures en stationnaire d'un vol : liste de fenêtres {gaz_pct, tr_min, tension_v, courant_a}."""
    if 'rcCommand[3]' not in c:
        return []
    t = (A[:, c['time']] - A[0, c['time']]) * 1e-6
    dt = float(np.median(np.diff(t)))
    n = int(FENETRE_S / dt)
    if n < 10:
        return []
    lo, hi = H.get('motorOutput', [48, 2047])[:2]
    mot = A[:, [c[f'motor[{i}]'] for i in range(rj.nb_moteurs(c))]]
    arme = (np.nan_to_num(A[:, c['flightModeFlags']]).astype(int) & 1) == 1
    champs = rj.colonnes_erpm(list(c), H)
    res = []
    for s in range(0, len(A) - n, n):
        sl = slice(s, s + n)
        gaz = A[sl, c['rcCommand[3]']]
        if not arme[sl].all() or gaz.mean() < 1100 or gaz.std() > STAT_GAZ_ECART:
            continue
        if max(np.abs(A[sl, c[f'rcCommand[{k}]']]).max() for k in range(3)) > STAT_MANCHE:
            continue
        if max(A[sl, c[f'gyroADC[{k}]']].std() for k in range(2)) > STAT_GYRO or (mot[sl] >= hi - 1).any():
            continue
        w = {'gaz_pct': (gaz.mean() - 1000) / 10}
        if champs and poles:
            w['tr_min'] = float(A[sl][:, [c[k] for k in champs]].mean() * 100 * 2 / poles)
        if 'vbatLatest' in c:
            w['tension_v'] = float(A[sl, c['vbatLatest']].mean() / 100)
        if 'amperageLatest' in c:
            w['courant_a'] = float(A[sl, c['amperageLatest']].mean() / 100)
        res.append(w)
    return res


# Centrage (protocole « centrage_v1 », commun avec le calculateur) : stationnaire calme sur deux caps opposés.
# Ordre des moteurs du Quad X de Betaflight : 1 arrière droit, 2 avant droit, 3 arrière gauche, 4 avant gauche.
POSITIONS_QUADX = ['arriere_droit', 'avant_droit', 'arriere_gauche', 'avant_gauche']
CENTRAGE = {
    'marge_bord_s': 2.0,          # écarté après l'armement et avant le désarmement
    'manche_moyen': 10,           # |rcCommand| moyen roulis / tangage (±500) : 2 % de la course
    'manche_max': 50,             # coup de manche : 10 %
    'lacet_max_dps': 20.0,        # rotation en lacet au-delà : segment écarté
    'cellule_min_v': 3.7,
    'gaz_ecart': 15,              # écart type des gaz : 1,5 %
    'cap_tolerance_deg': 30.0,    # demi-tour : 180 ± 30°
    'duree_min_s': 20.0,
    'cap_min_s': 5.0,             # chaque cap doit compter
    'inclinaison_max_deg': 3.0,
    'dispersion_max_pct': 5.0,    # écart type des régimes d'un moteur entre fenêtres (vent, rafales)
}


def segments_centrage(t, arme, rc, gaz, rpm, sortie, vbat, lacet_dps, acc, cellules, sat):
    """Fenêtres d'une seconde valides pour le centrage. Tableaux par échantillon : t (s), arme (bool), rc (n, 2)
    roulis / tangage en rcCommand, gaz (rcCommand 1000-2000), rpm (n, m) tr/min mécaniques, sortie (n, m) en %,
    vbat (V), lacet_dps (°/s), acc (n, 3) accéléromètre, sat (bool : un moteur saturé). Retourne une liste de
    fenêtres {t, cap_deg, rpm, sortie, inclinaison_deg, vbat}."""
    dt = float(np.median(np.diff(t)))
    n = max(10, int(FENETRE_S / dt))
    cap = np.cumsum(lacet_dps) * dt                       # cap intégré depuis le début du journal (°)
    # écarter le début et la fin de chaque période armée
    marge = np.zeros(len(t), bool)
    bords = np.flatnonzero(np.diff(arme.astype(int))) + 1
    debuts = [0] + list(bords); fins = list(bords) + [len(t)]
    for d, f in zip(debuts, fins):
        if arme[d]:
            marge[d:f] = (t[d:f] >= t[d] + CENTRAGE['marge_bord_s']) & (t[d:f] <= t[f - 1] - CENTRAGE['marge_bord_s'])
    ok = marge & ~sat & (np.abs(lacet_dps) <= CENTRAGE['lacet_max_dps']) & (rpm > 0).all(axis=1) \
        & (vbat / cellules >= CENTRAGE['cellule_min_v'])
    res = []
    for s in range(0, len(t) - n, n):
        sl = slice(s, s + n)
        # en vol seulement : gaz levés (drone armé posé au ralenti écarté), comme pour les mesures du stationnaire
        if not ok[sl].all() or gaz[sl].mean() < 1100 or gaz[sl].std() > CENTRAGE['gaz_ecart']:
            continue
        a = np.abs(rc[sl])
        if a.mean(axis=0).max() > CENTRAGE['manche_moyen'] or a.max() > CENTRAGE['manche_max']:
            continue
        g = acc[sl].mean(axis=0)
        incl = float(np.degrees(np.arctan2(np.hypot(g[0], g[1]), abs(g[2]))))
        res.append({'t': float(t[s]), 'cap_deg': float(cap[sl].mean()), 'rpm': rpm[sl].mean(axis=0),
                    'sortie': sortie[sl].mean(axis=0), 'inclinaison_deg': incl, 'vbat': float(vbat[sl].mean())})
    return res


def bloc_centrage(fenetres, positions, cellules):
    """Bloc « centrage » d'un vol à partir de ses fenêtres valides. Les deux caps sont regroupés autour du cap de
    la première fenêtre (cap A) et du cap opposé (cap B) ; le régime retenu par moteur est la moyenne des deux caps,
    ce qui retire l'effet du vent et d'un accéléromètre légèrement décalé (pas celui d'un moteur plus faible)."""
    refus = []
    if positions is None:
        return {'protocole': None, 'refus': ['disposition non prise en charge (Quad X seulement en version 2)']}
    if not fenetres:
        return {'protocole': None, 'refus': ['aucune fenêtre de stationnaire calme']}
    a0 = fenetres[0]['cap_deg']
    tol = CENTRAGE['cap_tolerance_deg']

    def ecart(c, ref):
        return abs((c - ref + 180.0) % 360.0 - 180.0)
    caps = {'A': [w for w in fenetres if ecart(w['cap_deg'], a0) <= tol],
            'B': [w for w in fenetres if ecart(w['cap_deg'], a0 + 180.0) <= tol]}

    def resume(lst):
        if not lst:
            return None
        r = np.array([w['rpm'] for w in lst]); o = np.array([w['sortie'] for w in lst])
        return {'duree_s': round(len(lst) * FENETRE_S, 1),
                'tr_min': {p: round(float(np.median(r[:, i])), 1) for i, p in enumerate(positions)},
                'tr_min_ecart_type': {p: round(float(r[:, i].std()), 1) for i, p in enumerate(positions)},
                'sortie_pct': {p: round(float(np.median(o[:, i])), 2) for i, p in enumerate(positions)},
                'inclinaison_deg': round(float(np.mean([w['inclinaison_deg'] for w in lst])), 2),
                'inclinaison_ecart_type_deg': round(float(np.std([w['inclinaison_deg'] for w in lst])), 2)}
    ca, cb = resume(caps['A']), resume(caps['B'])
    duree = (ca['duree_s'] if ca else 0) + (cb['duree_s'] if cb else 0)
    if duree < CENTRAGE['duree_min_s']:
        refus.append(f"{duree:.0f} s de stationnaire valide, il en faut {CENTRAGE['duree_min_s']:.0f}")
    if not cb or cb['duree_s'] < CENTRAGE['cap_min_s'] or ca['duree_s'] < CENTRAGE['cap_min_s']:
        refus.append(f"un seul cap (pas de demi-tour de 180 ± {tol:.0f}° avec au moins {CENTRAGE['cap_min_s']:.0f} s de chaque côté)")
    for nom, c in (('A', ca), ('B', cb)):
        if c and c['inclinaison_deg'] > CENTRAGE['inclinaison_max_deg']:
            refus.append(f"cap {nom} : inclinaison moyenne {c['inclinaison_deg']}° (plus de {CENTRAGE['inclinaison_max_deg']:.0f}°)")
        if c:
            disp = max(c['tr_min_ecart_type'][p] / c['tr_min'][p] * 100 for p in positions)
            if disp > CENTRAGE['dispersion_max_pct']:
                refus.append(f"cap {nom} : régimes trop dispersés ({disp:.1f} %, vent ou rafales)")
    tr_min = None
    if ca and cb:
        tr_min = {p: round((ca['tr_min'][p] + cb['tr_min'][p]) / 2, 1) for p in positions}
    tous = [w['vbat'] for w in sorted(caps['A'] + caps['B'], key=lambda w: w['t'])]
    return {
        'protocole': 'centrage_v1' if not refus else None,
        'disposition': 'quadx',
        'ordre_moteurs': {str(i + 1): p for i, p in enumerate(positions)},
        'tr_min': tr_min,
        'caps': {'A': ca, 'B': cb},
        'tension_debut_v': round(tous[0], 2) if tous else None,
        'tension_fin_v': round(tous[-1], 2) if tous else None,
        'cellules': cellules,
        'refus': refus,
    }


def centrage_vol(H, c, A, poles):
    """Bloc centrage d'un vol (journal chargé)."""
    nm = rj.nb_moteurs(c)
    champs = rj.colonnes_erpm(list(c), H)
    besoin = ['rcCommand[0]', 'rcCommand[1]', 'rcCommand[3]', 'gyroADC[2]', 'accSmooth[0]', 'vbatLatest']
    if not champs or not poles or any(k not in c for k in besoin):
        return {'protocole': None, 'refus': ['journal sans régime des moteurs, accéléromètre ou tension (ou pôles non mesurés)']}
    t = (A[:, c['time']] - A[0, c['time']]) * 1e-6
    lo, hi = H.get('motorOutput', [48, 2047])[:2]
    mot = A[:, [c[f'motor[{i}]'] for i in range(nm)]]
    arme = (np.nan_to_num(A[:, c['flightModeFlags']]).astype(int) & 1) == 1
    vbat = A[:, c['vbatLatest']] / 100
    v0 = vbat[arme][:50].mean() if arme.any() else vbat[:50].mean()
    cellules = int(np.ceil(v0 / 4.35)) if v0 > 0 else None
    if not cellules:
        return {'protocole': None, 'refus': ['tension de batterie absente']}
    fen = segments_centrage(t, arme, A[:, [c['rcCommand[0]'], c['rcCommand[1]']]], A[:, c['rcCommand[3]']],
                            A[:, [c[k] for k in champs]] * 100 * 2 / poles, np.clip((mot - lo) / (hi - lo), 0, 1) * 100,
                            vbat, A[:, c['gyroADC[2]']], A[:, [c[f'accSmooth[{k}]'] for k in range(3)]], cellules,
                            (mot >= hi - 1).any(axis=1) | (mot <= lo).any(axis=1))
    return bloc_centrage(fen, POSITIONS_QUADX if nm == 4 else None, cellules)


# Limites des mesures du fichier retour : une valeur hors limites est remplacée par null (jamais inventée)
LIMITES_RETOUR = {
    'poles': (4, 64), 'tr_min_stationnaire': (500, 100000), 'gaz_stationnaire_pct': (0, 100),
    'tension_stationnaire_v': (1.0, 70.0), 'courant_stationnaire_a': (0.1, 500.0), 'duree_stationnaire_s': (STAT_MIN_S, 100000),
    'saturation_pct': (0, 100), 'desynchronisations': (0, 100000), 'telemetrie_figee_pct_max': (0, 100),
}


def retour(chemins, lib, nom=None):
    """Mesures faites en vol, pour le calculateur (format talktrue-trueglide-retour, version 2 : version 1 + centrage)."""
    vols, fenetres, H0 = [], [], None
    for chemin in chemins:
        for i in range(1, Parser.load(chemin).reader.log_count + 1):
            v = analyser_vol(lib, chemin, i)
            if v:
                vols.append(v)
    if not vols:
        raise SystemExit('Aucun vol lisible dans les journaux donnés.')
    H0 = vols[0]['H']
    conc = conclure_poles([v['poles'] for v in vols if 'poles' in v])
    poles = int(conc[0]) if conc else None
    centrage = []
    for chemin in chemins:
        for i in range(1, Parser.load(chemin).reader.log_count + 1):
            d = charger(chemin, i)
            if d:
                fenetres += stationnaire(d[0], d[1], d[2], poles or d[0].get('motor_poles'))
                b = centrage_vol(d[0], d[1], d[2], poles)
                b['vol'] = f'{os.path.basename(chemin)} n° {i}'
                centrage.append(b)
    duree = len(fenetres) * FENETRE_S

    def med(cle):
        x = [w[cle] for w in fenetres if cle in w]
        return round(float(np.median(x)), 2) if duree >= STAT_MIN_S and len(x) >= STAT_MIN_S / FENETRE_S else None
    sat = [v['mesures']['saturation'] for v in vols if 'saturation' in v['mesures']]
    fig = [max(v['mesures']['figee']) for v in vols if 'figee' in v['mesures']]
    capteur = H0.get('currentSensor')
    m = {
        'poles': poles,
        'poles_exact': bool(conc[1]) if conc else None,
        # vitesse mécanique : avec les pôles MESURÉS seulement (avec un motor_poles faux elle serait fausse)
        'tr_min_stationnaire': med('tr_min') if poles else None,
        'gaz_stationnaire_pct': med('gaz_pct'),
        'tension_stationnaire_v': med('tension_v'),
        'courant_stationnaire_a': med('courant_a') if capteur else None,
        'duree_stationnaire_s': round(duree, 1) if duree >= STAT_MIN_S else None,
        'saturation_pct': round(float(np.mean(sat)), 2) if sat else None,
        'desynchronisations': int(sum(len(v.get('relances', [])) for v in vols)) if lib is not None and fig else None,
        'telemetrie_figee_pct_max': round(float(max(fig)), 2) if fig else None,
    }
    refuses = []
    for k, (a, b) in LIMITES_RETOUR.items():
        if m[k] is not None and not a <= m[k] <= b:
            refuses.append(f'{k} = {m[k]}')
            m[k] = None
    proto = H0.get('motor_pwm_protocol')
    return {
        'format': 'talktrue-trueglide-retour', 'version': 2,
        'source': {'logiciel': 'TrueGlide analyse_blackbox', 'firmware': H0.get('Firmware revision'),
                   'carte': H0.get('Board information'), 'nom': nom or H0.get('Craft name'),
                   'date': datetime.datetime.now(datetime.timezone.utc).replace(microsecond=0).isoformat(),
                   'vol': ', '.join(os.path.basename(x) for x in chemins) + f' ({len(vols)} vols lus)'},
        'mesures': m,
        # un bloc par vol lu ; protocole = "centrage_v1" seulement si le vol remplit le protocole, sinon null + refus
        'centrage': centrage,
        'reglages': {'motor_poles': H0.get('motor_poles'), 'motor_kv': H0.get('motor_kv'),
                     'motor_pwm_protocol': PROTOCOLES[proto] if isinstance(proto, int) and proto < len(PROTOCOLES) else None,
                     'dshot_bidir': bool(H0['dshot_bidir']) if 'dshot_bidir' in H0 else None,
                     'capteur_courant_offset_echelle': capteur},
        'notes': ([f'Hors limites, remplacé par null : {", ".join(refuses)}.'] if refuses else []) +
                 ['Stationnaire : fenêtres de 1 s armé, gaz stables (écart type < 1,5 %), manches au centre, drone stable, '
                  'moteurs non saturés ; valeurs = médiane des fenêtres.',
                  'Courant : capteur de la carte avec l\'offset et l\'échelle notés dans reglages (étalonnage non vérifié).'],
    }


def conseils_dshot(H):
    proto = PROTOCOLES[H['motor_pwm_protocol']] if isinstance(H.get('motor_pwm_protocol'), int) and H['motor_pwm_protocol'] < len(PROTOCOLES) else '?'
    boucle = 1e6 / (H.get('looptime', 125) * H.get('pid_process_denom', 1))
    lignes = []
    if H.get('dshot_bidir') and H.get('debug_mode') != rj.DEBUG_DSHOT_RPM_TELEMETRY and 'Betaflight 4.' in str(H.get('Firmware revision', '')):
        lignes.append('- Betaflight 4.4 / 4.5 n\'enregistre pas la vitesse des moteurs : pour la voir, mettre `set debug_mode = '
                      'DSHOT_RPM_TELEMETRY` avant de voler (désynchronisations et télémétrie figée seront alors analysées).')
    lignes += [f'- Protocole moteur : **{proto}**, boucle PID **{boucle / 1000:.0f} kHz**, DShot bidirectionnel : '
              f"**{'oui' if H.get('dshot_bidir') else 'non'}**."]
    if proto == 'DSHOT300' and boucle > 4100:
        lignes.append('- DShot300 avec une boucle PID au-dessus de 4 kHz : passer en DShot600 ou baisser la boucle à 4 kHz.')
    elif proto == 'DSHOT300':
        lignes.append('- DShot300 à 4 kHz est le bon choix avec le DShot bidirectionnel : DShot600 n\'apporte rien à 4 kHz et '
                      'rend la télémétrie plus sensible aux parasites (trame retour deux fois plus rapide).')
    elif proto == 'DSHOT600' and H.get('dshot_bidir'):
        lignes.append('- DShot600 bidirectionnel : si la télémétrie se fige ou si les moteurs saccadent, essayer DShot300 avec '
                      'une boucle PID à 4 kHz (pid_process_denom = 2).')
    return lignes


def rapport(chemins, lib):
    out = ['# Analyse des journaux de vol', '']
    for chemin in chemins:
        n = Parser.load(chemin).reader.log_count
        vols = [v for v in (analyser_vol(lib, chemin, i) for i in range(1, n + 1)) if v]
        if not vols:
            continue
        H = vols[0]['H']
        out += [f'## {os.path.basename(chemin)}', '',
                f"- Firmware : {H.get('Firmware revision', '?')} ; carte : {H.get('Board information', '?')} ; "
                f"nom : {H.get('Craft name', '?')}"]
        out += conseils_dshot(H)
        conc = conclure_poles([v['poles'] for v in vols if 'poles' in v])
        regle = H.get('motor_poles', 14)
        if conc is None:
            out += [f'- Pôles des moteurs : réglé à **{regle}** ; mesure impossible ou pas assez nette (il faut le gyro brut, '
                    'la vitesse des moteurs et quelques secondes de vol à gaz stables).']
        elif conc[0] == regle or (not conc[1] and abs(conc[0] - regle) <= 2):
            mesure = f'**{conc[0]}**' if conc[1] else f'environ **{conc[0]}** (±2)'
            out += [f'- Pôles des moteurs : réglé à **{regle}**, mesuré {mesure} dans les vibrations du gyro : '
                    + ('correct.' if conc[0] == regle else 'compter les aimants pour confirmer.')]
        else:
            out += [f'- **Pôles des moteurs : réglé à {regle}, mesuré {conc[0] if conc[1] else f"environ {conc[0]} (±2)"}** dans les vibrations du gyro. Le filtre RPM '
                    f'et le ralenti dynamique visent une mauvaise fréquence (facteur {regle / conc[0]:.2f}). Compter les aimants '
                    f'de la cloche puis `set motor_poles = <nombre d\'aimants>`.']
        out += [f"- Ralenti dynamique : {H.get('dyn_idle_min_rpm', 0) * 100} tr/min" + (' (désactivé)' if not H.get('dyn_idle_min_rpm') else ''),
                '', '| Vol | Durée (s) | Défauts constatés |', '|---|---|---|']
        for v in vols:
            out.append(f"| {v['idx']} | {v['duree']:.1f} | " + ('<br>'.join(v['defauts']) if v['defauts'] else 'aucun') + ' |')
        out.append('')
        # synthèse par moteur
        compte = {}
        for v in vols:
            for _, i, _, _ in v.get('relances', []):
                compte[i] = compte.get(i, 0) + 1
        if compte:
            pire = max(compte, key=compte.get)
            out += [f'**Synthèse** : {sum(compte.values())} désynchronisation(s) ou blocage(s), dont {compte[pire]} sur le '
                    f'moteur {pire + 1}.']
            if compte[pire] >= 0.7 * sum(compte.values()):
                out += [f'Un seul moteur concentre les défauts : panne matérielle probable du moteur {pire + 1} ou de sa voie '
                        'ESC (soudure d\'une phase, roulement, bobinage, MOSFET). Test : échanger ce moteur avec un autre ; '
                        'si le défaut suit le moteur, changer le moteur, sinon c\'est la voie ESC.']
            out.append('')
    return '\n'.join(out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('journaux', nargs='+'); ap.add_argument('-o', '--sortie')
    ap.add_argument('--retour', help='écrit les mesures de vol pour le calculateur (talktrue-trueglide-retour)')
    ap.add_argument('--nom', help='nom de la configuration dans le fichier retour (par défaut : nom du drone du journal)')
    a = ap.parse_args()
    try:
        lib = rj.bibliotheque()
    except Exception as ex:   # pas de gcc : analyse sans détection de désynchronisation
        print('Détection de désynchronisation indisponible :', ex, file=sys.stderr)
        lib = None
    if a.retour:
        r = retour(a.journaux, lib, a.nom)
        with open(a.retour, 'w') as f:
            json.dump(r, f, ensure_ascii=False, indent=2)
            f.write('\n')
        print(json.dumps(r['mesures'], ensure_ascii=False, indent=2))
        return
    texte = rapport(a.journaux, lib)
    if a.sortie:
        open(a.sortie, 'w').write(texte + '\n')
    print(texte)


if __name__ == '__main__':
    main()

# État de TRUEGLIDE — à lire en premier dans chaque discussion (développement et audit)

Mis à jour le 08/11/2026, 15 h 30. Modes d'emploi (hors ligne, mise en route depuis Betaflight, Studio, plan de vol) corrigés le 07/11 et donnés à Johann (hors dépôt). À mettre à jour à chaque changement (version en ligne, règle, défaut, audit, essai).
Détail et historique : `trueglide/REPRISE.md` et `trueglide/controle/RESULTATS.md`.

## 1. En ligne aujourd'hui

| Quoi | Version | Commit | Date |
|---|---|---|---|
| Logiciel /app/ | 83011e7 (copie exacte de /test/, main-C1-XdPzc.js ; essais au banc du 08/11 tous OUI, décision de Johann « OUI VALIDÉ … PUBLIER SUR APP ») | gh-pages 113fa92 ; configurateur main = version/app-2026-11-08 = 83011e7 | 08/11/2026 |
| Logiciel /test/ | 83011e7 (même bundle que /app/) | gh-pages 11d7568 | 08/11/2026 |
| Firmwares en ligne (catalogue de /app/ et /test/) | **cb4e3f0f2** (code = 5e246cc), 33 cartes, modèle TRUEGLIDE, version d'essai (seule SPEEDYBEEF405MINI essayée sur une vraie carte, révisions précédentes ; cb4e3f0 en essai TG-A1-7) ; précédente 4b3d47cba | gh-pages 674ceca (notes, flasheur, accueil) | 08/11/2026 |
| Onglet ESC | onglet/v18 (0.33.0-dev · da0ad00, OUI de l'audit) sur /app/ et /test/ ; v17 retirée (relance possible pendant « Déconnecter ») | TrueGlide-ESC-Configurator | 08/11/2026 |
| Firmware sur le Mini Pouce de Johann | cb4e3f0 (flashé le 08/11, = firmware/ en ligne) | Firmware-TrueGlide main | 08/11/2026 |

## 2. Règles permanentes de Johann

- 07/11 : « PUBLIE CORRIGE MET A JOUR JE VEUX PLUS D EWRREUR DANS MON LOGICIEL » et, dans la discussion de
  développement, « OUI CORRECTION MISE A JOUR AUDIT ». Sens : corriger, mettre sur /test/, faire passer l'audit
  indépendant ; sur OUI, publier le **logiciel** sur /app/ (et avancer main) sans redemander. Johann n'est sollicité que
  pour un essai au banc indispensable, en une ligne.
- 07/11 : « CA DOIT ETRE CHANGé AUTOMATIQUEMENT SANS MON ACCORD C EST FINI TU METS TOUT A JOUR » (firmwares), puis
  « ALORS ARRETE LE FIRMWARE », puis (07/11, annule l'arrêt) « NON ON REPRENDS LA COMPILATION CAR LE CONTROL AUDIT ESC
  SE FAIT VIA LE LOGICIEL ET PAS LE FIRMWARE » et ici « TU PEUX COMPILER ET FINIR LES MISE A JOUR POUR EN FINIR ? ».
  Sens : firmwares (.hex, index.json) comme le logiciel : compilés, publiés d'abord sur `firmware-test/`, audit ; sur
  OUI, catalogue `firmware/` mis à jour sans redemander, notes de version « version d'essai », seul le F405 Mini essayé
  sur une vraie carte.
- Toujours : essais sans hélices ; aucun secret affiché ; rien d'un autre produit dans TRUEGLIDE ; commits avec
  `[skip ci]`, sans identifiant de modèle d'IA ; un .hex donné à Johann garde son nom standard ; vérifier la révision
  dans le fichier avant de le donner.

## 3. Défauts encore ouverts

| Gravité | Défaut | Où |
|---|---|---|
| Choix gardé (audit F9) | Retour batterie : si le GPS Rescue revient après un état « retour impossible », le retour part tout de suite (batterie déjà basse, pilote déjà prévenu ; le manche l'annule) | firmware |
| Dit dans les notes (08/11) | power_guard_current 1 à 9 A sans minimum au firmware (banc) ; relance moteur sans limite de coupures ; retour batterie (F9) ; test moteurs arrêté après 1 s sans renvoi (logiciel du 08/11 requis) ; aucune fonction TrueGlide essayée en vol (TG-P3B) | firmware |
| À surveiller | Relance moteur : vérifiée seulement sans fausse alerte (TG-A1-4), pas sur un vrai moteur calé : garder désactivée par défaut | firmware |
| Corrigé et en ligne (08/11) | /app/ 94634f0 : en 3D (DShot), toucher un seul curseur envoyait 1000 (pleine marche arrière) aux autres moteurs (audit F1) | logiciel, onglet Moteurs |
| Corrigé sur /test/ 1cf71f8, audit en cours | F2 à F8 de l'audit du 07/11 (plage contrôlée avant l'envoi, interrupteur annulable, codes hors ligne refusés par numéro, réglages à risque complétés, onglet ESC une demande à la fois, mineurs) |  logiciel |
| Corrigé sur /test/ 98da639 puis 1cf71f8, audit en cours | CSP (sauf 'unsafe-eval' : Vue compile <app> au démarrage) ; fonds Google après accord ; polices du site hébergées sur le site ; failsafe « Tombe » confirmé ; power_guard_current 1 à 9 A confirmé (0 = arrêt ; minimum non imposé au firmware : 2 A sert au banc) | logiciel, site |
| D'origine Betaflight | 3D changé sans redémarrage | firmware |
| Mineur (prudent) | Interrupteur failsafe (BOXFAILSAFE) actif : « Liaison radio — » alors que la radio est liée | logiciel, Tableau de bord |
| À essayer | Onglet ESC : AM32 pas encore essayé sur un vrai ESC AM32 (lecture BLHeli_S / Bluejay essayée, TG-A1-6) | onglet ESC v16 |

## 4. Journal des audits

| Date | Version | Verdict | Lien |
|---|---|---|---|
| 06/11/2026 | logiciel avant ebde305 | 1er audit : PAS PRÊT, 15 constats | `controle/REPONSE_AUDIT_2026-11-06.md` |
| 06/11/2026 | ebde305 puis 17ca114 | contre-audit (N1 à N4…) | `controle/REPONSE_AUDIT_2026-11-06.md` |
| 06/11/2026 | onglet ESC v16 (29100f6) | vérification en ligne : v16 corrigée (discussion « Audit avant publication – TrueGlide ESC Configurator (PUBLIC) ») | TrueGlide-ESC-Configurator, branche audit/publication-public, docs/audit/VERIFICATION-DEPLOIEMENT-2026-11-06.md |
| 07/11/2026 | ebee781 (sécurité du test moteurs) + firmware ee050eb | OUI pour /app/ (M1 à M4 à faire) | `controle/AUDIT_2026-11-07_moteurs.md` |
| 07/11/2026 | firmwares 4b3d47cba, 33 cartes (sommes, nom de carte, révision contrôlés) | OUI pour firmware/ | gh-pages f0cebe4 |
| 07/11/2026 | 94634f0 (liaison radio « — » quand la carte signale la perte du signal) | OUI pour /app/ (mineur : interrupteur failsafe BOXFAILSAFE affiche aussi « — », sens prudent) | — |
| 07/11/2026 | 7b2dbcb (PID absents du diff all comparés aux valeurs par défaut du firmware) | OUI pour /app/ | gh-pages 0a4c259 |
| 08/11/2026 | 1cf71f8, a991709, fce3fc3 (onglet ESC v18), firmware 5e246cc puis 33 cartes cb4e3f0f2 | OUI pour /app/ (fce3fc3) et pour firmware/ (cb4e3f0f2, sommes, noms de carte et révision contrôlés) ; F9 gardé (choix) | gh-pages acf583d et 674ceca |
| 07/11/2026, 23 h | 94634f0 (recontrôle complet) | NON : F1 bloquant (3D marche arrière), F2 à F8, mineurs ; fiche TG-A1-7 à corriger (batterie pour lire les ESC, essai F1) | audit, message du 07/11 23 h 10 |
| 07/11/2026 | 391767f (molette, dialogues cachés, rxfail/PID, flasheur ; tests.yml exécuté avec succès) | OUI pour /app/ | REPRISE, 07/11 |

## 5. Essais au banc faits (vraie carte : Mini Pouce, SpeedyBee F405 Mini)

| Date | Fiche | Versions | Résultat |
|---|---|---|---|
| 28/10 | TG-P2 | 1791465 | 32 oui sur 32 |
| 29/10 | TG-P2B, TG-P3 | f9fc6d5 | oui (baromètre corrigé ; premiers vols) |
| 30/10 | TG-P3A | avant 24a71eb | A1 à A5 oui |
| 31/10 | TG-P2B, TG-P2C | 24a71eb | baromètre OK |
| 03/11 | TG-V1 (vol intérieur) | 24a71eb | tout oui |
| 07/11 | TG-A1 | 24a71eb + logiciel /test/ | toutes les questions oui (partie 5 refaite ; 6 = onglet ESC v16, 4 ESC BLHeli_S 35 A lus) |
| 07/11 | TG-A1-5 bis (3D + Sauvegarder, batterie branchée) | 24a71eb + ebee781 | aucun mouvement |
| 07/11 | TG-A1-5C (arrêt pendant le test, mode normal, pas 3D) | ee050eb + ebee781 | 3 oui |
| 08/11 | Fiches d'essais du 08/11 (audit) : 0.1, 0.2, TG-A1-7 (7-0, 7A, 7B, 7C-1 à 7C-5), onglet ESC v18 Q0, Q1-1 à 5, Q2-1 à 10, Q3-1 à 4 | cb4e3f0 + /test/ 83011e7 + ESC v18 da0ad00 | tout OUI (12 h 33 à 15 h 17) |
| — | TG-P3B (fonctions TrueGlide en vol) | — | PAS FAIT |

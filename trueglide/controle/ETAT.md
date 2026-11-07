# État de TRUEGLIDE — à lire en premier dans chaque discussion (développement et audit)

Mis à jour le 07/11/2026, 17 h 50. À mettre à jour à chaque changement (version en ligne, règle, défaut, audit, essai).
Détail et historique : `trueglide/REPRISE.md` et `trueglide/controle/RESULTATS.md`.

## 1. En ligne aujourd'hui

| Quoi | Version | Commit | Date |
|---|---|---|---|
| Logiciel /app/ | 391767f | gh-pages 79d224b (`main--ZG4MhpC.js`) ; main du configurateur = 391767f ; branche `version/app-2026-11-07` | 07/11/2026 |
| Logiciel /test/ | 391767f | gh-pages a9965fc | 07/11/2026 |
| Firmwares en ligne (catalogue de /app/ et /test/) | **4b3d47cba** (= ee050eb), 33 cartes, modèle TRUEGLIDE, version d'essai (seule SPEEDYBEEF405MINI essayée sur une vraie carte) ; précédente 24a71eb02 | gh-pages f0cebe4 (site et notes : f34f6fa) | 07/11/2026 |
| Catalogue d'essai `firmware-test/` (pour l'audit, lu par aucune page) | 4b3d47cba (= ee050eb), 33 cartes | gh-pages 04e699a | 07/11/2026 |
| Onglet ESC (/app/ et /test/) | onglet/v16 | TrueGlide-ESC-Configurator master 29100f6 | 06/11/2026 |
| Firmware sur le Mini Pouce de Johann | ee050eb (flashé, pas publié) | Firmware-TrueGlide main | 07/11/2026 |

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
| Mineur | Un PID absent du diff all (resté à sa valeur par défaut) ne déclenche la question « réglages à risque » qu'au-dessus de 150 | logiciel, write_guard.js |
| À surveiller | Relance moteur : vérifiée seulement sans fausse alerte (TG-A1-4), pas sur un vrai moteur calé : garder désactivée par défaut | firmware |
| À traiter | CSP, fonds de carte Google et polices Google chargés sans consentement | logiciel |
| À traiter | Failsafe DROP sans confirmation dans l'onglet Failsafe | logiciel |
| À traiter | power_guard_current sans minimum | firmware / logiciel |
| D'origine Betaflight | 3D changé sans redémarrage | firmware |
| À essayer | Onglet ESC : AM32 pas encore essayé sur un vrai ESC AM32 (lecture BLHeli_S / Bluejay essayée, TG-A1-6) | onglet ESC v16 |

## 4. Journal des audits

| Date | Version | Verdict | Lien |
|---|---|---|---|
| 06/11/2026 | logiciel avant ebde305 | 1er audit : PAS PRÊT, 15 constats | `controle/REPONSE_AUDIT_2026-11-06.md` |
| 06/11/2026 | ebde305 puis 17ca114 | contre-audit (N1 à N4…) | `controle/REPONSE_AUDIT_2026-11-06.md` |
| 06/11/2026 | onglet ESC v16 (29100f6) | vérification en ligne : v16 corrigée (discussion « Audit avant publication – TrueGlide ESC Configurator (PUBLIC) ») | TrueGlide-ESC-Configurator, branche audit/publication-public, docs/audit/VERIFICATION-DEPLOIEMENT-2026-11-06.md |
| 07/11/2026 | ebee781 (sécurité du test moteurs) + firmware ee050eb | OUI pour /app/ (M1 à M4 à faire) | `controle/AUDIT_2026-11-07_moteurs.md` |
| 07/11/2026 | firmwares 4b3d47cba, 33 cartes (sommes, nom de carte, révision contrôlés) | OUI pour firmware/ | gh-pages f0cebe4 |
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
| 07/11 | TG-A1-5C (arrêt pendant le test) | ee050eb + ebee781 | 3 oui |
| — | TG-P3B (fonctions TrueGlide en vol) | — | PAS FAIT |

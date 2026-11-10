# TRUEGLIDE : procédure de mise à jour depuis Betaflight

Document du 10/11/2026. À lire au début de chaque mise à jour TRUEGLIDE.
Il s'applique au firmware (`Jschmiedd/Firmware-TrueGlide`), au logiciel (`Jschmiedd/TrueGlide-Configurator`) et au site.

## 1. Principe

- TRUEGLIDE suit Betaflight, qui est sa base. Aujourd'hui : firmware **Betaflight 2026.6.2** (`e0b7bb01b`) et logiciel
  **Betaflight App 2026.6.2** (`091127c`).
- Betaflight sort environ **deux versions par an** : en juin et en décembre (2025.12, 2026.6…).
- TRUEGLIDE sort **deux mises à jour par an**, 1 à 2 mois après chaque version de Betaflight. Chacune reprend la base
  Betaflight et ajoute les améliorations et fonctions prévues.
- Entre deux mises à jour, seulement des **corrections** : petites versions de Betaflight (par exemple 2026.6.3),
  défauts trouvés, et toute correction de **sécurité**, tout de suite.

## 2. Calendrier

| Quand | Quoi |
|---|---|
| Juin | Betaflight sort 20XX.6. Je relève ce qui change et j'ouvre la branche de fusion. |
| Juillet | Fusion, tests automatiques, simulateur. Tu choisis les fonctions de la mise à jour. |
| Mi-août | Arrêt des nouvelles fonctions : on ne fait plus que tester et corriger. |
| Fin août | Audit indépendant, /test/, fiches au banc, vol d'essai, puis /app/ : **TRUEGLIDE 20XX.6**. |
| Décembre | Betaflight sort 20XX.12. Même chose. |
| Février | **TRUEGLIDE 20XX.12**. |

Prochaines : **TRUEGLIDE 2026.12** (février 2027, base Betaflight 2026.12) et **TRUEGLIDE 2027.6** (août 2027, base
Betaflight 2027.6).
Le numéro TRUEGLIDE reste celui de sa base Betaflight. Ainsi, on sait toujours sur quelle version il est construit.

## 3. Suivre Betaflight (Johann)

1. GitHub, dépôt `betaflight/betaflight` : bouton **Watch**, puis **Custom**, puis cocher **Releases**.
2. Pareil pour `betaflight/betaflight-configurator`.
3. À chaque e-mail « release », me l'envoyer en une ligne, par exemple « Betaflight 2026.12.0 est sorti ».
4. Garder une **liste des idées** pour la prochaine mise à jour : fonctions, améliorations, remarques d'essais.

## 4. Ce qui change entre TRUEGLIDE et Betaflight (mesuré le 10/11/2026)

| | Firmware | Logiciel |
|---|---|---|
| Fichiers Betaflight modifiés | 31 | 98 |
| Fichiers propres à TRUEGLIDE | 25 (+ 9 fichiers de tests) | 60 |
| Fichiers Betaflight retirés | 0 | 13 |
| Lignes | +2 332 / −33 | +16 365 / −2 893 |
| Difficulté de la fusion | faible : presque que des ajouts | moyenne : onglets refaits, traductions |

Le code TRUEGLIDE est surtout dans ses propres fichiers : Speeder, protections, garde moteur, retour batterie, relance
moteur, filtre du baromètre, carte hors ligne, Studio… Dans les fichiers Betaflight, chaque changement est marqué
« TrueGlide ». Avant chaque fusion, la liste exacte se refait avec :

```
git fetch --depth=1 https://github.com/betaflight/betaflight.git refs/tags/<ancienne base>:refs/tags/bf-base
git diff --name-status bf-base HEAD -- src/main mk Makefile
```

Fichiers Betaflight modifiés dans le firmware, à revoir un par un à chaque fusion :
`Makefile`, `mk/source.mk`, `blackbox/blackbox.c`, `build/debug.c`, `build/debug.h`, `build/version.h`,
`cli/settings.c`, `drivers/bus_i2c_busdev.c`, `drivers/dshot.c`, `drivers/dshot.h`, `drivers/motor.c`, `fc/core.c`,
`fc/rc.c`, `fc/rc_modes.h`, `fc/runtime_config.c`, `fc/runtime_config.h`, `flight/gps_rescue_multirotor.c`,
`flight/mixer.c`, `flight/pid.c`, `io/gimbal_control.c`, `msp/msp.c`, `msp/msp_box.c`, `osd/osd.h`,
`osd/osd_warnings.c`, `pg/pg_ids.h`, `rx/crsf_protocol.h`, `sensors/barometer.c`, `target/common_post.h`,
`target/common_pre.h`, `telemetry/crsf.c`, `telemetry/mavlink_mission.c`.

Points à surveiller en priorité : `flight/mixer.c` et `drivers/motor.c` (arrêt des moteurs, test des moteurs,
limite power_guard), `msp/msp.c` (garde moteur, MSP_SET_MOTOR), `fc/core.c` (armement, désarmement),
`cli/settings.c` et `pg/pg_ids.h` (numéros des réglages : jamais deux fois le même).

## 5. Procédure d'une mise à jour (développement)

1. **Branche** : `maj-betaflight-<version>` (firmware et logiciel), partie de `main`.
2. **Fusion** de l'étiquette Betaflight (par exemple `2026.12.0`). Résoudre chaque conflit en gardant le comportement
   Betaflight ET l'ajout TRUEGLIDE. Noter chaque conflit dans `REPRISE.md`.
3. **Relire les notes de version de Betaflight** : un changement qui touche une fonction TRUEGLIDE (moteurs, mixer, MSP,
   failsafe, GPS Rescue, OSD) est vérifié à la main.
4. **Construire** : firmware `make EXTRA_FLAGS=-Werror` pour les 33 cartes (et SITL), logiciel `npm run build`.
5. **Tests automatiques** : firmware `make test` (aujourd'hui 76 suites), logiciel `npx vitest run` (aujourd'hui 1 061
   tests) et `npx eslint`. Aucun test désactivé pour « passer ».
6. **Simulateur (SITL)** : `trueglide/sitl_bench/test_msp_motor_safety.py`, puis contrôle du vrai logiciel dans le
   navigateur (tous les onglets, carte hors ligne, test moteurs normal et 3D, aucune violation CSP).
7. **Nouvelles fonctions** de la liste de Johann, chacune avec son test, et **désactivée par défaut** si elle agit en
   vol.
8. **Audit indépendant** : il relit le diff et refait les tests. Il répond OUI ou NON, avec ses constats. On corrige,
   puis il recontrôle, jusqu'au OUI.
9. **/test/** et **firmware d'essai** (`firmware-test/`), vérifiés en ligne.
10. **Fiches au banc** (Johann, hélices retirées) : sécurité moteurs, 3D, ESC, carte hors ligne. Puis un **vol d'essai**.
11. Sur le OUI de Johann : **/app/** (copie exacte de /test/), catalogue `firmware/`, notes de version, `ETAT.md`.

## 6. Entre deux mises à jour

- **Petite version Betaflight** (par exemple 2026.6.3) : fusion, tests, audit, /test/, puis /app/. Sans nouvelle
  fonction.
- **Correction de sécurité** : tout de suite, sans attendre, avec la même chaîne (tests, audit, /test/, /app/).
- **Idées** : notées dans la liste pour la mise à jour suivante, pas ajoutées au milieu.

## 7. Qui fait quoi

| Johann | Développement | Audit indépendant |
|---|---|---|
| Suit les versions Betaflight (e-mails GitHub) | Fusion, construction, tests, simulateur | Relit, refait les tests |
| Choisit les nouvelles fonctions | Nouvelles fonctions et corrections | OUI / NON avec constats |
| Fiches au banc, vol d'essai | /test/, firmware d'essai, notes de version | Vérifie ce qui est en ligne |
| Dit « OUI » pour /app/ | /app/, `firmware/`, `ETAT.md` | |

## 8. Règles qui ne changent pas

- Hélices retirées pour tout essai au banc.
- Rien de publié sur /app/ ni dans `firmware/` sans le OUI de l'audit et de Johann.
- Une fonction qui agit en vol reste désactivée par défaut, tant qu'elle n'a pas été essayée en vol.
- Notes de version honnêtes : ce qui est essayé, ce qui ne l'est pas (« version d'essai »).
- Commits avec `[skip ci]`, à l'identité TrueGlide, sans identifiant de modèle d'IA ; aucun secret affiché ; rien d'un
  autre produit dans TRUEGLIDE.

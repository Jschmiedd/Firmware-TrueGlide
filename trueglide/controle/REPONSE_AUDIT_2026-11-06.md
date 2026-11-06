# Réponse à l'audit indépendant du 06/11/2026

Pour l'auditeur suivant : chaque constat bloquant ou majeur, ce qui a été fait, où, et comment c'est prouvé.
Logiciel : dépôt TrueGlide-Configurator, branche `essai-onglets` (commits 2d5185a et ebde305), en ligne sur la page
d'essai https://jschmiedd.github.io/TrueGlide-Configurator/test/ ; **pas encore sur /app/** (attend l'essai au banc
de Johann). Firmware : ce dépôt, branche `main` (d0908fa).

Légende des preuves : **E** = vérifié par exécution (tests automatiques ou essai réel dans le navigateur avec le
simulateur SITL) ; **L** = vérifié par lecture du code ; **M** = à vérifier sur le matériel (fiche d'essai au banc).

| N° | Constat | Fait | Où | Preuve |
|---|---|---|---|---|
| 1 | Arrêt des moteurs à 1000 en 3D (pleine marche arrière) | Arrêt au neutre (`zeroThrottleValue`) partout : quitter l'onglet, enregistrer, assistants d'ordre et de sens | `src/components/tabs/MotorsTab.vue` | E : test `trueglide_audit_securite` (audit 1) ; M : 3D non essayé sur un ESC |
| 2 | CLI hors ligne et Studio rejouent `motor`, `serialpassthrough`, `msc`… | Liste blanche des commandes de réglage ; refus partout (CLI par MSP, onglet CLI en texte, Studio, envoi) | `src/js/trueglide/cli_safety.js`, `offline/board.js`, `TrueGlideStudioTab.vue`, `offline/send.js` | E : tests (audit 2, onglet CLI) ; navigateur : `motor` et `serialpassthrough` refusés, `set` gardé |
| 3 | Codes MSP du fichier carte non filtrés | Fichier refusé à l'ouverture s'il contient une écriture immédiate ou inconnue ; contrôle refait avant l'envoi | `offline/safety.js`, `offline/store.js`, `ConnectButton.vue` | E : tests (audit 3) |
| 4 | Pas de `diff all` automatique avant écriture | `diff all` de la vraie carte téléchargé sur l'ordinateur avant toute écriture (envoi hors ligne et Studio) ; restauration : coller le fichier dans le CLI puis `save` | `src/js/trueglide/write_guard.js` | E : navigateur + SITL, fichier `<drone>_sauvegarde_avant_envoi_<date>.txt` reçu |
| 5 | L'envoi écrase les changements faits depuis la récolte | Comparaison du `diff all` de la carte avec la récolte (ou la dernière relecture) ; question avec les lignes différentes ; annuler = rien d'écrit | `offline/send.js`, `TrueGlideOfflineBox.vue` | E : navigateur + SITL (`small_angle` changé : question affichée, annulation sans écriture) |
| 6 | Coupure USB : état partiel sans avertissement ; coupure pendant `save` prise pour une réussite | Coupure pendant l'envoi : message (rien enregistré, réglages seulement en mémoire vive) ; après `save` : attente de la reconnexion, relecture, alerte si la carte est revenue aux réglages d'usine | `write_guard.js` (`saveAndVerify`, `looksLost`) | E : tests `trueglide_write_guard` ; **M : relecture après `save`** (le SITL ne se reconnecte pas seul) |
| 7 | /app/ charge l'onglet ESC v15 « en essai » | Retour en v14 (dernière version essayée sur un ESC réel) jusqu'à un essai AM32 | `src/js/trueglide/esc_tab.js` | E : navigateur, la page v14 est chargée |
| 8 | Firmware non signé ; adresse de remplacement lue dans le stockage | Adresse de remplacement lue seulement sur un serveur local (toutes les pages de jschmiedd.github.io partagent le stockage). **Signature non faite** : voir plus bas | `firmware_source.js` | E : test (audit 8) |
| 9 | Carte cible contrôlée par le nom du fichier seulement | Nom de la carte cherché dans le contenu du .hex (chaîne BOARD_NAME) : fichier renommé refusé ; carte branchée absente du fichier : oui explicite demandé | `firmware_source.js` (`hexContainsBoardName`), `FirmwareFlasherTab.vue` | E : tests (audit 9) ; navigateur : hex MATEKF405SE renommé en SPEEDYBEEF405MINI refusé |
| 10 | Filtre du baromètre toujours actif, sans test ; README « tout désactivé » | Filtre sorti dans un fichier testé, comportement identique ; README précisé (seule correction toujours active) | `src/main/sensors/baro_step_filter.c`, `src/test/unit/baro_step_filter_unittest.cc`, `README.md` | E : test unitaire 3/3 ; construction SPEEDYBEEF405MINI `-Werror` OK |
| 11 | Relance moteur : TG-P3A-3 à refaire ; pas de limite aux coupures | Décision de Johann (06/11) : **garder** les nouveaux essais tant qu'un moteur bloqué ne repart pas (protège l'ESC). Site corrigé : la relance n'est plus dite vérifiée sur la version en ligne | `site/index.html`, `trueglide/REPRISE.md` | **M : TG-P3A-3 à refaire** |
| 12 | Fiches d'essai vides | Résultats rassemblés avec leurs preuves ; TG-P3B marquée « pas faite » | `trueglide/controle/RESULTATS.md` | L |
| 13 | Aucune CI | Lint réparé (il contrôlait le logiciel construit `src/dist`) ; workflow « Tests » chaque nuit + à la demande (lint, 968 tests, construction) ; firmware : tests unitaires + Studio + construction chaque nuit | `.github/workflows/tests.yml` (logiciel), `.github/workflows/trueglide.yml` (firmware) | E : lint et tests en local ; premier passage nocturne à regarder dans l'onglet Actions |
| 14 | npm audit : 66 failles (3 critiques) | Mises à jour (Capacitor 8.6.0, DOMPurify 3.4.17, Vue 3.5.45, Vite 8.3.6) et `overrides` (tiptap 3.33.0, prosemirror-view, undici, fast-uri, brace-expansion, browserslist) : **0 critique, 0 haute**, 9 faibles ou moyennes dans les outils de construction | `package.json`, `package-lock.json` | E : `npm audit --omit=dev` ; tests et construction OK ; navigateur OK |
| 15 | Script de construction des carte.wasm absent (GPL) | Les sources étaient publiques sur la branche `essai-vraie-carte` (35b1d53), sans lien : adresse exacte ajoutée sur `main` et dans le logiciel | `trueglide/SOURCES_CARTES_SIMULEES.md`, `simulee/LISEZMOI.txt` (logiciel) | L |

## Non fait, et pourquoi

- **8, signature des firmwares** : les firmwares et l'application sont servis par le même dépôt GitHub Pages. Qui peut
  modifier la liste des firmwares peut aussi modifier l'application et retirer la vérification : une signature ne
  protégerait que si les deux étaient hébergés séparément. Elle exigerait en plus une clé privée à conserver hors du
  code, du chat et de GitHub (règle de Johann) ; perdue, plus aucune mise à jour possible. Ce qui protège : HTTPS, somme
  SHA-256 de chaque fichier dans la liste, contrôle du nom de carte dans le contenu du .hex (constat 9). À reprendre si
  les firmwares sont un jour hébergés ailleurs que l'application.

## À vérifier sur le matériel (fiche d'essai au banc, sans hélices)

1. Envoi hors ligne vers la vraie carte en USB : sauvegarde téléchargée, puis relecture après `save` (constat 6).
2. Relance moteur TG-P3A-3 sur le firmware actuel (constat 11).

# Réponse au contre-audit du 06/11/2026 (versions logiciel 4adf6a4, firmware de ce commit)

Logiciel : branche `essai-onglets`, commits 5271b5f et 4adf6a4, en ligne sur /test/ (main-Chmb5sh9.js). 978 tests,
lint et construction OK. Essais dans le navigateur avec le simulateur SITL : question « réglages à risque » affichée
(MSP_SET_MIXER_CONFIG, MSP_SET_RX_MAP…) puis annulation sans écriture, envoi accepté, sauvegarde listée dans le
Tableau de bord, CLI hors ligne, 12 onglets sans erreur.

| Point | Fait / pas fait | Où | Preuve |
|---|---|---|---|
| N1 /app/ sans les corrections | **Pas fait** : /app/ ne se publie que sur l'ordre écrit de Johann (« publie sur /app/ »), après la fiche TG-A1 | — | — |
| N2 arrêt à 3d_neutral (1460) en DShot + 3D | Fait : `motorStopValue` = 1500 en DShot + 3D, 3d_neutral seulement en PWM ; utilisé pour tous les arrêts et le départ des curseurs ; limite au banc centrée sur 1500 | `motor_bench.js`, `MotorsTab.vue` | E : test de comportement qui reproduit `dshotConvertFromExternal` (1460 → marche arrière ; valeur choisie → commande d'arrêt) |
| N3 commande cachée derrière un « set » (\r, \n) | Fait : tout caractère de contrôle refusé dans `isSettingCommand` (donc CLI hors ligne, Studio, envoi, ouverture d'un fichier carte) | `cli_safety.js` | E : test (\n, \r, \0, \t, ESC) |
| N4 confirmation spécifique | Fait : Studio montre la liste complète ; question à part pour mixer, mmix, smix, resource, timer, dma, map, aux, feature 3D, failsafe_*, yaw_motors_reversed, motor_output_reordering, 3d_*, motor_pwm_protocol, align_board_*, small_angle, runaway_takeoff_*, mixer_type, avec la ligne d'avant lue dans la sauvegarde ; envoi hors ligne : aussi pour les écritures d'onglet à risque (avant = « comparer avec la sauvegarde ») ; aussi dans l'onglet Réglage | `write_guard.js` (`riskyChanges`, `riskyQuestion`), `offline/send.js`, `TrueGlideStudioTab.vue`, `TrueGlideTuneTab.vue`, `TrueGlideOfflineBox.vue` | E : tests ; navigateur |
| Constat 4 partiel | Fait : sauvegarde avant écriture et relecture aussi pour l'onglet TrueGlide, la caméra et l'onglet Réglage ; sauvegarde gardée aussi dans le navigateur (10 dernières), téléchargeable depuis le Tableau de bord si le navigateur a bloqué le téléchargement | `TrueGlideTab.vue`, `TrueGlideTuneTab.vue`, `write_guard.js` (`keepBackup`) | E : tests ; navigateur (liste affichée) |
| Constat 6 messages | Fait : coupure USB = « rien d'enregistré ; batterie branchée : changements actifs jusqu'au redémarrage, débrancher avant de voler » ; relecture réglage par réglage (`get` de chaque `set` hors changement de profil, autres lignes cherchées dans le diff relu) ; « tous en place » seulement si chaque réglage relu a la valeur envoyée ; sinon nombre non relus ou écarts, alerte | `write_guard.js` (`verifySent`, `verificationMessage`), locales | E : tests (écart détecté, jamais « en place » sans relecture) ; **M** : relecture sur la vraie carte (TG-A1-1) |
| Constat 11 | Pas annoncé : site « relance en essai » ; Johann a choisi de garder les nouveaux essais (A) ; essai TG-A1-4 à faire | `site/index.html` | M |
| Constat 12 | Pas fait : les journaux Blackbox restent chez Johann (dépôt public) ; les résultats sont dans RESULTATS.md ; TG-A1 sera remplie | — | — |
| Constat 13 | **Pas fait** : `main` du logiciel = /app/ (règle de Johann) ; le workflow arrivera sur `main` avec la prochaine publication /app/ ; les commits restent « [skip ci] » (règle de Johann) | — | — |
| Constat 8 | Pas fait, raison plus haut ; acceptation écrite demandée à Johann | — | — |
| Mineurs | Fait : site (« vole comme Betaflight » nuancé, liste des envois décrite exactement, AM32 « pas encore essayé », relance « en essai »). Pas fait : CSP et fonds de carte / polices Google (plan de vol), failsafe DROP sans confirmation dans l'onglet Failsafe (comportement Betaflight), minimum de power_guard_current (0 = arrêt ; 2 A sert aux essais au banc), limite au banc hors curseurs | `site/index.html` | — |

Fiche matériel : `TG-A1_mode_operatoire.md` couvre W2 (commande moteur jamais envoyée), W3 (envoi hors ligne complet,
sauvegarde, question « carte changée », question « réglages à risque », relecture), W5 (restauration), W7 (ESC v14),
W8 (TG-P3A-3). Pas dans la fiche : W1 (3D sur ESC : demande un réglage 3D des ESC de Johann), W4 (coupure USB : l'envoi
dure quelques secondes, impossible à viser à la main), W6 (DFU : flash complet de la carte) ; leurs comportements sont
couverts par les tests et les essais dans le navigateur.

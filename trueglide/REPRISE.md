# Reprise du travail TrueGlide (à lire en début de conversation)

Une conversation par phase de test. Lire ce fichier, puis seulement les fichiers utiles à la phase.

## Produits et dépôts

- Firmware : `Jschmiedd/Firmware-TrueGlide` (public), base Betaflight 2026.6.2.
  Trois modèles : **TRUEGLIDE** (base), **TRUENAV** (combiné), **TRUENAV_LIGHT** (navigation).
- Logiciel : `Jschmiedd/TrueGlide-Configurator` (public), base Betaflight App 2026.6.2.
- En ligne : https://jschmiedd.github.io/TrueGlide-Configurator/ (site explicatif : accueil + `docs/`, une page par
  onglet), `/app/` (logiciel), `/firmware/` (manifeste de la mise à jour en ligne). Branche `gh-pages` du dépôt du
  logiciel. `/test/` et `/site-test/` redirigent vers les versions officielles depuis le 23 octobre 2026.
- Produit privé réécrit : `TrueGlide-XPro`, traité dans ses propres conversations (ne pas mélanger).

## Règles

- Réponses en français. Noms des produits en majuscules et en gras.
- Rien ne change par défaut ; jamais de chiffre faux affiché ; tester en vrai avant d'annoncer.
- Commits : `git -c user.name=TrueGlide -c user.email=jschmiedd@users.noreply.github.com commit -m "... [skip ci]"`,
  sans identifiant de modèle d'IA ni ligne Co-Authored-By. PR seulement sur demande de Johann.
- « Betaflight » reste volontairement dans les en-têtes blackbox et les descripteurs USB (compatibilité des outils).

## Firmware

- Chaîne ARM : `make arm_sdk_install` (ou xpack arm-none-eabi-gcc 13.3.1 dans le PATH).
- Construire : `make -j4 EXTRA_FLAGS=-Werror MODELE=TRUEGLIDE|TRUENAV|TRUENAV_LIGHT CONFIG=<CARTE>`
  → `obj/<modele>_<version>_<MCU>_<CARTE>.hex`. Copier chaque .hex tout de suite : `make clean` efface les autres modèles.
- Révision dans le firmware seulement si l'arbre git est propre (sinon « norevision »).
- Tests : `make EXTRA_FLAGS=-Werror test-all` (70 suites) ; Studio : `node --test trueglide/studio/test/studio.test.js`.
- SITL : `make MODELE=TRUEGLIDE CONFIG=SITL`, lancer `obj/main/trueglide_SITL.elf` (TCP 5761) sans `eeprom.bin`.
- Publication de la mise à jour en ligne : `python3 trueglide/outils/publier_firmware.py --obj <dossier .hex> --sortie <gh-pages>`.

## Logiciel

- `npm ci`, puis `npx eslint src test/js scripts --ignore-pattern "src/dist/**"`, `npx vitest run test/js`, `npm run build` → `src/dist`.
- Ne jamais passer prettier sur `src/components/tabs/TrueGlideTuneTab.vue`.
- Publier : copier `src/dist` dans `app/` de la branche `gh-pages`, commit, push.
- Connexion au SITL depuis le navigateur : pont WebSocket 5800 → TCP 5761, port manuel `ws://localhost:5800`.

## Phases de test

1. **Banc** : SITL + logiciel (connexion, onglets TrueGlide, sauvegarde, mise à jour en ligne).
2. **Première carte** : SpeedyBee F405 Mini, flash **TRUEGLIDE**, contrôles à plat sans hélices (capteurs, moteurs, OSD, alertes).
3. **Premiers vols** **TRUEGLIDE** : journaux blackbox + `diff all` + description du drone → analyse.
4. **TRUENAV** / **TRUENAV_LIGHT** : GPS, maintien de position, retour au départ, missions.
5. **Toutes les cartes** : compilation des 629 cartes par GitHub Actions et publication dans la mise à jour en ligne.

Noter à la fin de chaque phase, ci-dessous, ce qui est validé et ce qui reste.

## État au 14 octobre 2026 (fin de la première discussion)

Fait et en ligne :
- Renommage complet TrueFlight → **TRUEGLIDE** (firmware, logiciel, vitesse, journaux, dépôts renommés par Johann).
- 6 .hex de test (SpeedyBee F405 Mini et T-Motor Velox F7 SE, 3 modèles chacun) ; mise à jour en ligne avec contrôle SHA.
- Logiciel : barre du haut façon ancienne Betaflight (choix du port + connexion automatique, boutons ronds
  « Flasher le Firmware » et « Connexion » au sigle USB, rouge une fois connecté) ; logo rond refait proprement
  (CONTROL & FLY entier, sans coins de cadre ni traits sur le bord) sur l'accueil, les icônes et la vitrine ;
  filigrane TRUEGLIDE gris (sans disque) dans l'onglet CLI.
- Navigation mobile corrigée (menu, défilement), dialogue Options qui ne s'ouvre plus tout seul.

En attente de Johann :
- Feu vert pour compiler les 629 cartes (phase 5).
- Journaux blackbox F7 + `diff all` + description du drone (phase 3).
- Points juridiques pour un avocat : identifiant BTFL, réutilisation des presets, droits sur le code écrit avec une IA,
  dépôt de la marque à l'IPI.

## Décisions

- Ligne « Entering CLI Mode… » du firmware : laissée telle quelle (décision de Johann, 14 octobre).
- Salle blanche, exception (Johann, 27 octobre) : la forme et la mise en page des documents de contrôle peuvent venir
  de XPro (gabarit vide : sections, colonnes, en-tête, présentation) pour une documentation uniforme. Jamais le
  contenu (étapes, valeurs, textes techniques) ni du code : le contenu TrueGlide est rédigé ici.

## Journal des phases

- 22 octobre 2026 (deuxième discussion), en attente de validation par Johann, rien de remplacé en ligne :
  - Logiciel : branche `essai-onglets`, publiée sur `/test/` (gh-pages). Onglet Speeder avec le schéma du V2V 3",
    onglet Installation avec les vues Profil / Dessus (numéros des moteurs pour le Quad X seulement), environ 40 textes
    faux ou périmés corrigés (fr et en), TRUENAV_LIGHT reconnu par la commande `version`, « Annuler l'étalonnage »
    réparé, limites alignées sur le firmware, bouton d'assistance du CLI masqué. Testé sur le simulateur.
  - Site explicatif (accueil + une page par onglet) : branche `essai-site`, dossier `site/`, publié sur `/site-test/`.
    Construire : `python3 site/outils/construire.py` ; contrôles : `verifier_liens.py`, `verifier_rendu.cjs` ;
    captures : `site/outils/captures.cjs` (simulateur neuf à chaque thème).
  - Firmware : `.hex` SEQURE H743 V2 (3 modèles) compilés, non publiés. ESC SEQURE E160 G2 : pont V → V+ (16 V) et
    `battery_meter = ESC` nécessaires (voir la discussion).
  - Phase 2 (F405 Mini à plat) : pas encore faite.
  - Phase 5 (toutes les cartes), révision `1791465a6` : TRUEGLIDE 621/621, TRUENAV 606, TRUENAV_LIGHT 611 (les autres
    débordent la mémoire, liste dans LISEZMOI). Exclues : 5 cartes RP2350 et 3 SITL. Tous les `.hex` dans les Releases
    `essai-1791465a6-<MODELE>` du dépôt firmware (workflow `trueglide.yml`, cartes=TOUTES, job `version`).
    Liste de test `firmware-test/` sur gh-pages (627 fichiers : TRUEGLIDE pour toutes les cartes, les trois modèles
    pour SPEEDYBEEF405MINI, TMOTORVELOXF7SE, SEQUREH7V2), lue par `/test/` seulement ; `/firmware/` (lu par `/app/`)
    inchangé. Nom de carte avec tiret (LUXHDAIO-G4) accepté par le publieur et le logiciel. Compilé ≠ testé.
- 23 octobre 2026 : publication décidée par Johann (« oui tu peux tout remplacer »).
  - `/app/` construit depuis la branche `essai-onglets` (f7bd6da) du logiciel ; `main` du logiciel n'a pas été modifié.
  - `/firmware/` = l'ancienne liste de test : révision 1791465a6, TRUEGLIDE pour 621 cartes, les trois modèles pour
    SPEEDYBEEF405MINI, TMOTORVELOXF7SE, SEQUREH7V2. `firmware-test/` supprimé.
  - Page d'accueil = site explicatif (branche `essai-site`, dossier `site/`, construit par `site/outils/construire.py`).
  - Studio : onglet Ports (UART), fichier drone, fichier du calculateur `talktrue-trueglide-studio`, base du réglage
    « données du calculateur » (43 tests). CHIRP : pas ajouté, besoins à définir (document de suivi).
  - Fiches de salle blanche envoyées à XPro : 1/4 à 4/4, 5/6, 6/6, navigation 1/3 à 3/3.
  - Phase 2 (F405 Mini à plat) : pas encore faite. Rien n'a volé.
- 23 octobre 2026 (soir), demandes de Johann :
  - Batterie basse (firmware a43f068, désactivée par défaut) : `battery_arm_min_cell_voltage` (armement refusé,
    motif LOW_BATTERY, bit 29 ; ARM_SWITCH passe au bit 30), `battery_rth_cell_voltage` + `battery_rth_delay` (retour
    GPS RESCUE, annulé au manche > 30 % jusqu'au désarmement, OSD BAT RTH / BAT RTH N/A). Jamais de coupure en vol.
    7 tests unitaires ; le simulateur n'a pas de batterie : pas encore essayé en vol. Recompilation des cartes à faire
    pour la prochaine version.
  - Onglet ESC du logiciel (essai-onglets 47b7507) : iframe de TrueGlide ESC Configurator figé
    (https://jschmiedd.github.io/TrueGlide-ESC-Configurator/onglet/v1/), sans fusion ; une seule connexion USB.
    Publié sur /test/ (qui redevient une page d'essai, liste de firmwares officielle) ; messages ready / disconnected
    vérifiés en ligne ; pas encore essayé avec une vraie carte ni un vrai flash d'ESC.
  - Analyse des journaux : fichier de retour `talktrue-trueglide-retour` v2 (centrage) pour le calculateur.
- 25 octobre 2026, demande de Johann (« la lecture des broches c'est un point sécurité ? si oui intègre-le ») :
  - Studio lit les lignes `resource` d'un dump all / diff all : broches d'usine des 629 cartes dans `cartes.js`
    (champ `b`, `tools/extract_cartes.py`), `TFPorts.lireResources` et `TFPorts.carteLue` ; alertes « broche
    partagée » et « pas de broche » dans l'onglet Ports. 45 tests ; essayé dans la page (Chromium). Pas mis en
    ligne : la publication de Studio attend que les produits soient bien séparés (XPro / TrueGlide).

  - Mise en ligne demandée par Johann (« prépare pour publier, je veux tester Studio sur la vraie page ») : Studio en
    essai sur https://jschmiedd.github.io/TrueGlide-Configurator/studio/ (4c8916b, bandeau « version d'essai »).
    Site : section « L'écosystème TrueGlide » en bas de l'accueil, XPRO compris (nom et une ligne), batterie basse et
    onglet ESC marqués en essai, page Moteurs « Régler ou flasher les ESC » (essai-site 2c398b9, gh-pages d01cb1e).
  - Onglet Studio dans TrueGlide Configurator (essai-onglets d21ff26, publié sur /test/) : iframe de /studio/, avec
    ou sans carte ; carte branchée, « Lire la configuration de la carte » lit le diff all (CLI sur MSP) et le donne à
    Studio (message postMessage, rien n'est écrit dans la carte). Essayé en ligne avec le simulateur : 49 lignes reçues.
  - Studio (cc25cf5) : formulaire vide au départ (exemples en grisé), calcul refusé tant que les 5 champs du drone
    ne sont pas remplis, bouton « Nouveau drone ». Accueil du site : schéma de l'écosystème repris du site XRIG.
  - Studio en USB (Web Serial, js/usb.js) à la demande de Johann (« tout ce qui est fait dans Studio doit pouvoir se
    faire avec le navigateur ») : lecture du diff all puis exit (aucun enregistrement), envoi des commandes après
    confirmation (arrêt et exit à la première erreur, sinon save). Essayé avec le firmware SITL par un faux port série
    relié au CLI : lecture 48 lignes, envoi enregistré et relu, erreur volontaire = rien enregistré. Réglage complet :
    arrêt sur dshot_bidir (absent du SITL seulement). Pas encore essayé sur une vraie carte.

- 25 octobre 2026 (soir), demande de Johann : régler la carte hors ligne avec les mêmes onglets que le Configurator.
  - Solution A (Johann) : « carte hors ligne » dans TrueGlide Configurator (essai-onglets 2d88b8b, /test/ ef98b0f).
    Récolte carte branchée (le logiciel ouvre chaque onglet, garde les réponses), carte hors ligne dans la liste des
    ports (mêmes onglets), écritures relues comme le firmware (règles lues dans msp.c), envoi des changements préparés
    à la carte récoltée seulement. SITL : 5 changements hors ligne envoyés puis récoltés à nouveau, réponses identiques.
    Limites : profils non changeables hors ligne, Plan de vol non récolté, OSD / LED / servos / tables VTX pas encore
    vérifiés au SITL ; pas encore essayé sur une vraie carte.
- 26 octobre 2026 : un seul outil (décision de Johann). Studio est livré dans TrueGlide Configurator (copie studio/ par
  scripts/sync-trueglide-studio.mjs, aussi sans Internet), onglet Studio avec « Envoyer à la carte » par le logiciel
  (essai-onglets 9e2a942). La page /studio/ ouvre /test/#studio. Parcours complet refait au SITL en images :
  suivi/Parcours_TrueGlide_Configurator_Studio.pdf (18 pages). Pas encore essayé sur une vraie carte.
- 26 et 27 octobre 2026 : carte hors ligne sur le vrai firmware, et Betaflight.
  - Carte simulée (firmware exact de la carte compilé pour le navigateur) : branche firmware `essai-vraie-carte`
    (`trueglide/essai_navigateur/vraie_carte.py`), carte simulée Betaflight 2025.12.2 de la KarRmen dans
    `trueglide/betaflight/` (patch, options de construction, `construire.sh`) : `dump all` identique à la vraie carte
    (992 réglages sur 992). Logiciel essai-onglets c74d6d0 sur /test/ : une carte simulée par firmware, préparation
    16 s, « Sauvegarder » sans redémarrage compté, nouvelle récolte confirmée si des changements ne sont pas envoyés.
  - Essai de Johann sur sa vraie KarRmen (Betaflight 2025.12.2) : réglage préparé hors ligne, envoyé, lu dans
    Betaflight Configurator, puis changé dans Betaflight et relu par TrueGlide : communication validée.
  - Studio (branche firmware `studio-betaflight`) : firmware de la carte lu (TrueGlide ou Betaflight), réglages de
    Betaflight seulement sur une carte Betaflight (`settings_db_betaflight.js`), onglet Ports en Betaflight.
  - Onglet PID : chiffres et curseurs ensemble ; curseurs alignés. Tableau de bord : firmware Betaflight bien nommé.
  - Phase 2 (F405 Mini flashée en TRUEGLIDE, à plat) : toujours pas faite. Rien n'a volé en TRUEGLIDE.
  - Fiches de contrôle (forme des fiches XPro, contenu TrueGlide) : `trueglide/controle/`. Source
    `<CODE>_mode_operatoire.md`, fiche Word `<CODE>_fiche.docx` par `npm ci && node fiche.mjs <source>`. Première :
    TG-P2 (phase 2, F405 Mini à plat). LibreOffice affiche à tort les cases « non » cochées ; Word les montre vides.
  - Méthode de contrôle (Johann, 28 octobre) : chaque fiche est d'abord déroulée en simulation dans le vrai logiciel
    en ligne, avec le firmware exact de la carte (carte simulée, `trueglide/essai_navigateur/vraie_carte.py`, construite
    sur la révision publiée) et sa configuration ; une photo par question, ce qu'il faut regarder entouré en rouge,
    mise dans la fiche. TG-P2 : `trueglide/controle/TG-P2_derouler.cjs` (24 points vérifiés, 0 échec ; 12 demandent le
    vrai matériel : radio, moteurs, lunettes, batterie), photos dans `trueglide/controle/photos/TG-P2/`.
- 28 octobre 2026 : phase 2 faite par Johann sur sa vraie KarRmen (SpeedyBee F405 Mini), TRUEGLIDE 2026.6.2 (1791465).
  - Fiche TG-P2 : 32 « oui » sur 32. Flash avec XPro (le firmware XPro 0.8.6-banc ne passe pas seul en mode flash
    depuis TrueGlide Configurator) ; `diff all` Betaflight recollé sans ERROR ; `diff all` final identique sauf
    `acc_calibration` (recalibré) et `feature LED_STRIP` (activée par l'effacement complet, désactivée sous Betaflight).
  - Écarts relevés : la carte a 8 Mo de mémoire Blackbox (0x00ef4017), la carte simulée et la question 8.1 disent 16 Mo
    (à corriger) ; tension 14,80 V contre 14,92 V au multimètre (échelle 110, comme sous Betaflight) ; batterie vue
    une fois en 1S quand elle est branchée après l'USB (détection d'origine Betaflight).
  - Logiciel essai-onglets 31549b5 (pas encore publié) : intensité lue signée (−0,02 A affichait 655,34 A) ; « Chute de
    tension » devient « Écart avec la batterie pleine », « — » si négatif.
  - Baromètre : sous TRUEGLIDE (.hex 1791465, construction complète sans CLOUD_BUILD), mesures fausses (≈ −65 400 m,
    50 ms) au sol, 4 dans 3 enregistrements, et « zéro » d'altitude faux à chaque démarrage (680 à 2 122 m) ; altitude
    OSD jusqu'à −5 704 m / +587 m. Sous Betaflight 2025.12.2 (3 vidéos .osd, ≈ 5 min) et sous Betaflight 2026.6.2
    officiel en ligne (e0b7bb01b, même config ligne par ligne, 13,8 s) : aucune. Le baromètre est bon ; le défaut vient
    du .hex TRUEGLIDE (le code TrueGlide ne touche ni le baromètre, ni l'I2C, ni l'ordonnanceur ; autre différence :
    construction complète, avec magnétomètre sur le même I2C). Cause exacte non trouvée.
  - Correction (def66f9 puis f9fc6d5) : pression hors 300 à 1 200 hPa ignorée, et saut de plus de 20 m d'une mesure à
    l'autre ignoré (5 au plus de suite) ; rejouée sur les journaux : les 4 mesures fausses écartées, les normales
    gardées. .hex SPEEDYBEEF405MINI f9fc6d5 donné à Johann ; pas encore vérifié sur la carte. Cause d'origine inconnue.
  - Décision de Johann (29 octobre) : aucune autre carte reconstruite pour le moment (ni 42 courantes, ni 629). Les .hex
    publiés en 1791465 ont le défaut du baromètre.
  - Méthode : pour tout défaut, comparer d'abord avec Betaflight officiel sur la même carte et la même config.
  - Décision de Johann (29 octobre) : plus de photos en simulation dans les fiches (trop long). Fiches texte seulement :
    TG-P2B (contrôle du baromètre f9fc6d5, préalable) et TG-P3 (premiers vols, fonctions TrueGlide désactivées).
  - TG-P2B faite par Johann le 29 octobre (f9fc6d5, debug_mode = BARO) : 8 « oui » sur 8. Journal 20261029_093729
    (29,7 s armé) : 3 mesures fausses (à 5,3 s, 10,3 s et 12,4 s), toutes écartées ; baroAlt 1,6 à 3,0 m ; altitude OSD
    (.osd AvatarG0026) −1,0 à +0,4 m. Correction validée sur la vraie carte. Dans les mesures fausses, pression ET
    température sont fausses ensemble (−7 540 hPa et 258 à 275 °C) : les 6 octets lus du DPS310 sont faux en bloc,
    souvent à 5 s d'écart. Cause d'origine à chercher (lecture I2C), non bloquante pour TG-P3.
  - TG-P3 (premiers vols, f9fc6d5, config identique à Betaflight sauf acc_calibration) faite par Johann le 29 octobre :
    fiche 16 « oui », 2.2 (batterie pleine) non cochée. Journal 20261029_162404 : 2 vols en ANGLE (27,5 s et 22,3 s) et
    1 en ACRO (28,8 s), batterie au départ 14,5 à 15,0 V (pas pleine). Suivi des commandes 1 à 3 °/s, moteurs
    équilibrés, pas d'oscillation, aucune mesure fausse du baromètre. Vibration du roulis à la fréquence des moteurs
    (≈ 240 Hz, gyro brut σ ≈ 35 °/s, filtré 4 à 7 °/s) : hélice ou moteur à contrôler. Creux du baromètre de 2 à 6 m
    (50 à 200 ms) en stationnaire bas, au moment des accélérations verticales : souffle des hélices probable, à comparer
    avec un vol Betaflight avant ALTHOLD (TG-P4). Chocs de 5 à 8 g à chaque atterrissage (rebonds). Radio : manche
    gauche (Yaw) réglé, vis D. Note : le champ flightModeFlags du Blackbox contient les interrupteurs (bit 0 = ARM).


## 30/10/2026 — TG-P3A A2 : température des ESC à 0 °C une fois armé

- OSD G0028 (A2) : « ESC HOT » jamais affiché, élément température ESC à 0 °C pendant l'armement.
- Cause : la configuration de Johann a `dshot_edt = OFF` (défaut BETAFLIGHT). La carte n'envoie alors pas la commande
  « télémesure étendue » aux ESC à l'armement (`fc/core.c`), et `dshotCleanTelemetryData()` efface tout à l'armement :
  la température reste à 0. `power_guard` (température ≤ 0 = inconnue) et `crsf_tlm_esc_temp` (envoi seulement si
  chaque ESC donne une température) n'agissent donc pas. Ce n'est pas un défaut de code : réglage manquant dans la fiche.
- Correction : TG-P3A (A2, A4) et TG-P3B (2, 4) mettent `set dshot_edt = ON` pendant l'essai, puis `OFF` à la fin.
- Origine des 19/27/18/21 °C vus dans l'onglet Moteurs au repos : pas encore expliquée (avec EDT OFF, la carte décode
  tout comme des tours/min). À regarder avec `dshot_telemetry_info` au prochain essai.
- À faire : le Tableau de bord ne devrait pas montrer la protection température comme active si `dshot_edt = OFF`.
- Corrigé (30/10) : Tableau de bord du configurateur (d370128, essai-onglets, pas encore publié) : alerte sous
  « Protection anti-surchauffe » si une limite de température est réglée avec `dshot_edt = OFF`. Fiches TG-P3A-4 et
  TG-P3B-4 : manipulation radio RadioMaster Pocket (EdgeTX) détaillée. Non vérifié : qu'EdgeTX affiche bien la trame
  CRSF de température 0x0D (la fiche demande la version d'EdgeTX si le capteur n'apparaît pas).

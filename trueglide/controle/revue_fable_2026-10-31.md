# Revues Fable du 31 octobre 2026 : erreurs et corrections

Firmware : dépôt Firmware-TrueGlide, branche `main`. Configurateur : dépôt TrueGlide-Configurator, branche
`essai-onglets`. Chaque test ajouté échoue sans sa correction et passe avec elle (vérifié). Tests unitaires du firmware :
72 suites OK. Tests du configurateur : 917 OK. Construction ARM avec `-Werror` OK.

## Firmware TRUEGLIDE

| N° | Erreur trouvée | Correction appliquée | Commit | Preuve |
|---|---|---|---|---|
| F1 | Retour batterie : le mode GPS RESCUE pouvait s'enclencher sans que le rescue tourne (gaz à zéro, manches ignorés) | Retour seulement si le GPS RESCUE est configuré **et** disponible ; sinon `BAT RTH N/A`, le pilote garde la main | 253de3b | test `NoReturnWhenTheRescueDoesNotRun` |
| F2 | Baromètre DPS310 : lectures fausses (−7540 hPa, 258 °C) après une erreur I2C | Pilote I2C F4 remis à zéro après une erreur (l'adresse du registre était sautée) ; lecture rejetée sur erreur ; erreur attribuée au bon capteur (bus partagé) | 1341bd2, a01092b, d5244da, 253de3b | compilation ARM ; **à confirmer au banc : fiche TG-P2B** |
| F3 | Motif d'armement `LOW_BATTERY` affiché `ARM_SWITCH` dans les outils Betaflight | Gardé : `ARM_SWITCH` doit rester le dernier motif, tout autre emplacement donnerait un faux nom à un motif Betaflight. TrueGlide Configurator l'affiche juste ; documenté | b1d35cb (doc) | `status` en CLI donne le vrai motif |
| F4 | Le .hex contient toutes les options de la carte, pas seulement celles de la construction en ligne Betaflight | Texte corrigé : même code de vol, options en plus inactives tant qu'on ne les règle pas. Changer la construction = décision de Johann | b1d35cb (doc) | — |
| F5 | Une trame DShot étendue (température) rendait la vitesse moteur « fraîche » | Vitesse fraîche seulement sur une nouvelle trame de vitesse valide | 253de3b | test `ExtendedFrameDoesNotMakeTheSpeedFresh` |
| F6 | Relance moteur : non prévue en 3D ; ramenait le moteur à l'arrêt avec le ralenti dynamique | Désactivée en 3D ; moteur ramené au vrai ralenti | 1341bd2, b1d35cb (doc) | lecture du code, compilation (pas de test du mixeur dans Betaflight) |
| F7 | Le compteur d'erreurs I2C de `status` ne comptait que les temporisations | Erreurs de l'interruption comptées aussi ; la fiche TG-P2B les relève **après** l'essai armé | 253de3b, 4571055 | fiche TG-P2B |
| F8 | Retour batterie annulé dans le même cycle si le pilote tenait déjà un manche | Annulation seulement par un nouveau geste ; `BAT RTH OFF` 3 s à l'OSD | 63d0b1a | test `StickHeldWhenTheReturnStartsDoesNotCancelIt` |
| F9 | Limite de courant réglée sous le courant de stationnaire : gaz bloqués au plancher | Avertissement dans l'aide du réglage et dans la documentation | 397587f (conf.), b1d35cb | — |
| F10 | Ajouts sans nom dans les outils Betaflight (SPEEDER, debug, alertes OSD) | Tableau « Avec les outils Betaflight d'origine » dans la documentation | b1d35cb | — |
| F11 | Le workflow construisait sans `-Werror` | `EXTRA_FLAGS=-Werror` pour chaque carte | 253de3b | workflow |
| — | Studio, analyse : réglage de profil lu dans un autre profil que le profil actif | Valeur prise dans le profil actif | 972f8bf | test `studio.test.js` |
| — | Alertes TrueGlide (ESC HOT, AMP LIM, RESTART, BAT RTH OFF) pouvaient cacher LAND NOW | Placées après LAND NOW, liaison, rescue, LOW BATTERY | 1341bd2, a01092b | test OSD `TestElementWarningsTrueGlide` |
| — | Température ESC série : dernière valeur gardée d'un ESC muet | Lue par ESC, seulement si récente | 1341bd2 | compilation |
| — | Limite ESC / courant : pas appliquée en ALT HOLD, ou appliquée pendant une descente de secours | Appliquée en ALT HOLD, jamais en GPS RESCUE, failsafe ni descente de secours de mission | 63d0b1a, a01092b | compilation |

## TrueGlide Configurator

| N° | Erreur trouvée | Correction appliquée | Commit | Preuve |
|---|---|---|---|---|
| C1 | Intensité batterie lue non signée (−0,02 A affichée 655,34 A), branche `main` | Corrigé dans `essai-onglets` ; arrive sur `main` à la fusion | 31549b5 | test existant |
| C2 | Assistant, Analyse : valeur « actuelle » d'un réglage de profil prise dans un autre profil | Profil actif | 397587f | test `trueglide_analyse_profil` |
| C3 | Flasheur : fichier TrueGlide pour une autre carte = simple encadré | Confirmation oui / non obligatoire ; carte vérifiée aussi en série | 397587f | test `trueglide_firmware_source` (détection) ; fenêtre : essai dans l'application à faire |
| C4 | Hors ligne : écriture des compteurs de tension / courant corrompait la lecture | Compteur repéré par son id ; écriture trop longue signalée « pas relue » | 397587f | 2 tests `trueglide_offline` |
| C5 | Hors ligne : changements envoyés dans le profil actif au moment de l'envoi | Profils gardés à la récolte ; envoi refusé s'ils ont changé | 397587f | test `trueglide_offline_identity` |
| C6 | `main` : TRUENAV_LIGHT pris pour TRUENAV | Corrigé dans `essai-onglets` ; arrive sur `main` à la fusion | (essai-onglets) | test `trueglide_firmware_source` |
| C7 | `main` : motif `LOW_BATTERY` sans nom | Corrigé dans `essai-onglets` ; arrive sur `main` à la fusion | (essai-onglets) | test existant |
| C8 | Assistant : « Appliquer » écrit ~20 réglages sans confirmation ; « Boucle PID » affichée mais pas écrite | Confirmation avec la liste ; ESC et cellules à confirmer avant ; texte « Boucle PID possible » | 397587f | essai dans l'application à faire |
| C9 | `main` : cartes avec un tiret écartées de la liste en ligne | Corrigé dans `essai-onglets` ; arrive sur `main` à la fusion | (essai-onglets) | test `LUXHDAIO-G4` |
| C10 | Flasheur : fichier Betaflight pour une autre carte pas contrôlé | Carte lue dans le nom `betaflight_…` ; même confirmation | 397587f | test `trueglide_firmware_source` |
| C11 | `main` : 10 fichiers de tests ne se chargeaient pas | Corrigé dans `essai-onglets` ; arrive sur `main` à la fusion | (essai-onglets) | 917 tests OK |
| C12 | Configuration : horizon et cap masqués sur TRUEGLIDE | Affichés sur tous les firmwares | 397587f | compilation |
| C13 | Onglet ESC : page externe chargée en direct | Non corrigé ici (page hors de ce dépôt) : à intégrer au configurateur plus tard | — | — |
| — | Tableau de bord : protections affichées actives sans DShot EDT, capteur de courant ou vitesse moteur | Alertes ajoutées ; RSSI, armement, température, chute de tension « — » tant qu'inconnus | d370128, cf8b799 | tests `trueglide_dashboard`, `trueglide_settings` |
| — | Envoi refusé : réglages déjà envoyés gardés, « Enregistré » affiché quand même | Redémarrage sans sauvegarde ; « Enregistré » seulement après réussite | cf8b799 | test `trueglide_settings` |

## Reste à faire

- **TG-P2B** au banc avec 253de3b : confirme F2 et F7 (baromètre stable, erreurs I2C relevées).
- **TG-P3B** en vol : limite de courant, température ESC, relance moteur, Speeder.
- Fusion `essai-onglets` → `main` du configurateur (C1, C6, C7, C9, C11) : sur demande de Johann.

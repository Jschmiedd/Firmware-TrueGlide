# Essai TG-B1 : Betaflight App et TRUEGLIDE sur la même carte Betaflight, puis baromètre (DSHOT_TELEMETRY_COUNTS)

Date : …… Drone : Mini Pouce …… Fait par : ……

**HÉLICES RETIRÉES, drone fixé, gaz bas (1050 à 1100).** Si un moteur ne s'arrête pas : **débrancher la batterie tout
de suite**. Chaque essai moteur se **filme au téléphone, écran ET moteurs dans l'image**, du clic jusqu'à l'arrêt. Les
vidéos serviront aux signalements à Betaflight, puis à ta vidéo publique.

Pourquoi la carte passe en Betaflight : avec le firmware TRUEGLIDE, la carte coupe seule les moteurs 1 s après le
dernier ordre du logiciel. Elle cacherait donc le défaut de Betaflight App. Sur Betaflight officiel, seul le logiciel
compte, et la comparaison est juste : même carte, même firmware, deux logiciels.

Valeurs attendues vérifiées dans le code le 13/11 : Betaflight App 2026.6.2 (`MotorsTab.vue` : molette sans contrôle du
curseur choisi ; `useMotorTesting.js` et fenêtres moteurs : rien quand la page est cachée) ; firmware Betaflight
2026.6.2 (`MSP_SET_MOTOR` : la valeur reste tant qu'aucune autre n'arrive, sans limite de temps) ; TRUEGLIDE /app/
`7dc6865` (molette seulement sur le curseur cliqué ; page cachée = test coupé et fenêtres fermées).

## Avant de commencer (rien à cocher)

1. Garder sous la main la sauvegarde TRUEGLIDE `MINI_POUCE_TRUEGLIDE_diff_all_2026-11-08_cb4e3f0.txt` (pour la partie 4).
2. Téléphone sur un support, qui filme l'écran et le drone ensemble. Batterie chargée.
3. Navigateur Chrome ou Edge, une seule fenêtre.

## TG-B1-1. Carte en Betaflight 2026.6.2 officiel

1. Batterie débranchée. Ouvrir **app.betaflight.com**, onglet « Flasher le firmware » : carte SPEEDYBEEF405MINI,
   version **2026.6.2**, **effacement complet** activé, flasher.
2. « Connexion ». Onglet CLI, taper une ligne à la fois puis Entrée : `set motor_pwm_protocol = DSHOT300`,
   `set dshot_bidir = ON`, `set yaw_motors_reversed = ON`, `save`. Ne coller **aucun** ancien `diff all`.
3. Reconnecter, onglet CLI : `version`.

- La ligne affichée commence par « # Betaflight / STM32F405 » et contient 2026.6.2 ?

## TG-B1-2. Betaflight App : molette (FILMER)

Ce qu'on vérifie : si faire défiler la page au-dessus des curseurs change les moteurs. Ce qu'on regarde : le moteur 1
et le chiffre de son curseur. Danger : moteur qui accélère sans qu'on touche au curseur.

1. Batterie branchée. Onglet Moteurs, « Je comprends les risques » sur oui.
2. Tirer le curseur du MOTEUR 1 à la souris vers 1100, puis cliquer dans une zone vide de la page (pour ne plus avoir
   de curseur choisi).
3. Sans cliquer, poser la souris sur le curseur du moteur 1 et tourner la molette **vers le haut, 2 crans**.
4. « Je comprends les risques » sur non. Le moteur s'arrête.

- À l'étape 3, la page n'a PAS défilé et le curseur du moteur 1 est monté (environ 1150) ?
- À l'étape 3, le moteur 1 a accéléré ?
- À l'étape 4, le moteur s'est arrêté ?

## TG-B1-3. Betaflight App : page cachée (FILMER)

Ce qu'on vérifie : si le test continue quand la page n'est plus visible. Ce qu'on regarde : les moteurs pendant que
l'autre onglet est ouvert. Danger : moteurs qui tournent sans surveillance.

1. « Je comprends les risques » sur oui, curseur « Maître » vers 1100 : les moteurs tournent.
2. Ouvrir un nouvel onglet du navigateur (Ctrl + T). Compter **5 secondes**. Revenir sur Betaflight App.
3. « Je comprends les risques » sur non.

- À l'étape 2, les moteurs ont continué à tourner pendant les 5 s ?
- Au retour, le test était toujours actif (interrupteur sur oui, moteurs qui tournent) ?
- À l'étape 3, les moteurs se sont arrêtés ?

## TG-B1-4. Betaflight App : fenêtre du sens de rotation DShot (FILMER)

Ce qu'on vérifie : si la fenêtre continue quand la page est cachée. Danger : moteur qui tourne sans surveillance.

1. Onglet Moteurs, bouton du sens de rotation des moteurs (assistant DShot). Accepter l'avertissement, faire tourner
   un moteur comme l'assistant le propose.
2. Pendant qu'il tourne : nouvel onglet (Ctrl + T), 5 secondes, retour.
3. Fermer la fenêtre de l'assistant. Batterie débranchée.

- À l'étape 2, le moteur a continué à tourner et la fenêtre était toujours ouverte au retour ?
- À l'étape 3, le moteur s'est arrêté ?

## TG-B1-5. TRUEGLIDE /app/ sur la même carte Betaflight (FILMER)

Mêmes gestes, avec TRUEGLIDE. La carte reste en Betaflight 2026.6.2 : seules les protections du logiciel agissent.

1. Fermer Betaflight App. Ouvrir **TRUEGLIDE /app/** (version 7dc6865 en bas à droite), « Connexion ». Batterie
   branchée, onglet Moteurs, « Je comprends les risques » sur oui.
2. Molette : refaire TG-B1-2 étapes 2 et 3 (moteur 1 vers 1100, clic dans le vide, molette vers le haut sans cliquer).
3. Page cachée : curseur « Maître » vers 1100, nouvel onglet, 5 s, retour.
4. Sens DShot : refaire TG-B1-4 étapes 1 et 2.
5. Tout arrêter, batterie débranchée.

- À l'étape 2, la page a défilé et le moteur 1 n'a PAS changé de vitesse ?
- À l'étape 3, les moteurs se sont arrêtés dès l'ouverture du nouvel onglet, et au retour le test était sur non ?
- À l'étape 4, la fenêtre s'est fermée et le moteur s'est arrêté dès l'ouverture du nouvel onglet ?

## TG-B1-6. Retour en TRUEGLIDE cb4e3f0

1. Batterie débranchée. TRUEGLIDE /app/, onglet « Flasher le firmware » : SPEEDYBEEF405MINI, version en ligne
   **cb4e3f0**, **effacement complet**, flasher.
2. « Connexion », onglet CLI : coller tout le fichier `MINI_POUCE_TRUEGLIDE_diff_all_2026-11-08_cb4e3f0.txt`, Entrée.
   La carte redémarre.
3. Reconnecter, CLI : `version`, puis `get craft_name`.

- `version` contient cb4e3f0 et `craft_name` = MINI POUCE ?

## TG-B1-7. Baromètre et télémesure DShot (batterie, armé au banc, hélices retirées)

Ce qu'on vérifie : si les télémesures coupées en pleine trame (debug[2]) arrivent en même temps que des erreurs I2C
(cause amont betaflight#15533). Ce qu'on regarde : le compteur d'erreurs I2C de `status`. Danger : aucun particulier,
hélices retirées.

1. Onglet Blackbox : « Effacer la mémoire flash ».
2. CLI : `get dshot_bitbang` (noter la réponse), puis `set debug_mode = DSHOT_TELEMETRY_COUNTS`, `save`.
3. Reconnecter, CLI : `status`, noter la ligne « I2C=… (… errors) ».
4. Déconnecter. Batterie branchée, armer à la radio, gaz vers le stationnaire, **3 minutes** sans à-coups. Désarmer,
   batterie débranchée.
5. Reconnecter, CLI : `status`, noter la ligne I2C. Onglet Blackbox : enregistrer le journal.
6. CLI : `set debug_mode = BARO`, `save`.

| Relevé | Valeur |
|---|---|
| `get dshot_bitbang` | |
| Ligne I2C avant (étape 3) | |
| Ligne I2C après (étape 5) | |

- Le journal Blackbox est enregistré (nom du fichier noté) ?

## À envoyer

### Les vidéos TG-B1-2 à TG-B1-5 (noms des fichiers)

### Le journal Blackbox de TG-B1-7 et les deux lignes I2C [grand]

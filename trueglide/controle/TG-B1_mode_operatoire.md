# Essai TG-B1 : Betaflight App et TRUEGLIDE sur la même carte Betaflight, puis mesure du baromètre (avant correction)

Date : …… Drone : Mini Pouce …… Fait par : ……

**HÉLICES RETIRÉES, drone fixé, gaz bas (1050 à 1150).** **Arrêt le plus rapide** dans les deux logiciels : appuyer
sur la touche **Espace** (toute touche coupe le test, sauf flèches haut/bas, Page préc./suiv., Alt et Tab). Si un
moteur ne s'arrête pas : **débrancher la batterie tout de suite**. Chaque essai moteur se **filme au téléphone, écran ET
moteurs dans l'image**, du geste jusqu'à l'arrêt.

Pourquoi la carte passe en Betaflight : avec le firmware TRUEGLIDE, la carte coupe seule les moteurs 1 s après le
dernier ordre du logiciel, ce qui cacherait le défaut de Betaflight App. Sur Betaflight officiel, l'ordre envoyé aux
moteurs reste **sans limite de temps** : si l'USB se coupe ou si le logiciel se bloque, les moteurs continuent. Garder
la main près de la prise batterie pendant les parties 2 et 3.

Pendant les parties 2 à 4 : **souris à molette crantée** (pas de molette libre, pas de pavé tactile), **aucune touche
du clavier** sauf pour arrêter (une touche coupe le test et fausserait l'essai). Changer d'onglet **à la souris
seulement**. L'assistant « sens de rotation DShot » n'est **pas** utilisé dans cet essai : il réécrit le sens enregistré
dans les 4 ESC.

Valeurs attendues vérifiées dans le code (13/11, puis audit) : Betaflight App 2026.6.2 et master : molette sans
contrôle du curseur choisi (+25 par cran), rien quand la page est cachée ; firmware Betaflight 2026.6.2 :
`MSP_SET_MOTOR` sans limite de temps ; TRUEGLIDE /app/ `7dc6865` : molette seulement sur le curseur cliqué, page
cachée = test coupé.

## Avant de commencer (rien à cocher)

1. Garder sous la main la sauvegarde TRUEGLIDE `MINI_POUCE_TRUEGLIDE_diff_all_2026-11-08_cb4e3f0.txt` (pour la partie 5).
2. Téléphone sur un support, qui filme l'écran et le drone ensemble. Batterie chargée. Smoke Stopper si tu en as un,
   pour le premier branchement de la batterie après le flash.
3. Navigateur Chrome ou Edge, **une seule fenêtre**, avec **un deuxième onglet déjà ouvert** (n'importe quelle page),
   pour pouvoir changer d'onglet à la souris.

## TG-B1-1. Carte en Betaflight 2026.6.2 officiel

1. Batterie débranchée. Ouvrir **app.betaflight.com** dans le premier onglet. Noter la version de Betaflight App
   (en bas de l'écran) dans le tableau de la partie 6. Onglet « Flasher le firmware » : carte SPEEDYBEEF405MINI,
   version **2026.6.2**, **effacement complet** activé, flasher.
2. « Connexion ». Onglet CLI, taper une ligne à la fois puis Entrée : `set motor_pwm_protocol = DSHOT300`,
   `set dshot_bidir = ON`, `set yaw_motors_reversed = ON`, `save`. Ne coller **aucun** ancien `diff all`.
3. Reconnecter, onglet CLI : `version`.
4. Brancher la batterie (Smoke Stopper si tu en as un). Aucun bruit anormal, aucun moteur qui tourne.

- La ligne affichée commence par « # Betaflight / STM32F405 » et contient 2026.6.2 ?
- À l'étape 4, aucun moteur n'a tourné ?

## TG-B1-2. Betaflight App : molette (FILMER)

Ce qu'on vérifie : si la molette change un moteur alors qu'aucun curseur n'est choisi. Ce qu'on regarde : le moteur 1
et le chiffre de son curseur. Danger : moteur qui accélère sans qu'on ait choisi son curseur.

1. Onglet Moteurs, « Je comprends les risques » sur oui (à la souris).
2. Tirer le curseur du MOTEUR 1 à la souris vers 1100, puis cliquer dans une zone vide de la page (plus aucun curseur
   choisi).
3. Sans cliquer, poser la souris sur le curseur du moteur 1 et tourner la molette **vers le haut, 2 crans**.
4. Appuyer sur **Espace**.

- À l'étape 3, le curseur du moteur 1 est monté d'environ 50 (vers 1150) et le moteur 1 a accéléré ?
- À l'étape 4, le test s'est coupé et le moteur s'est arrêté ?

## TG-B1-3. Betaflight App : page cachée (FILMER)

Ce qu'on vérifie : si le test continue quand la page n'est plus visible. Ce qu'on regarde : les moteurs pendant que
l'autre onglet est affiché. Danger : moteurs qui tournent sans surveillance (main près de la prise batterie).

1. « Je comprends les risques » sur oui, curseur « Maître » à la souris vers 1100 : les moteurs tournent.
2. **À la souris**, cliquer sur le deuxième onglet du navigateur. Compter **5 secondes**. Revenir à la souris sur
   l'onglet Betaflight App.
3. Appuyer sur **Espace**. Batterie débranchée.

- Pendant les 5 s, les moteurs ont continué à tourner ?
- Au retour, le test était toujours actif (interrupteur sur oui, moteurs qui tournent) ?
- À l'étape 3, les moteurs se sont arrêtés ?

## TG-B1-4. TRUEGLIDE /app/ sur la même carte Betaflight (FILMER)

Mêmes gestes, avec TRUEGLIDE. La carte reste en Betaflight 2026.6.2 : seules les protections du logiciel agissent.

1. « Déconnecter », fermer l'onglet Betaflight App. Dans le premier onglet, ouvrir **TRUEGLIDE /app/** (version
   7dc6865 en bas à droite), « Connexion ». Batterie branchée, onglet Moteurs, « Je comprends les risques » sur oui.
2. Molette : moteur 1 vers 1100 à la souris, clic dans une zone vide, puis molette vers le haut, 2 crans, sans cliquer.
3. Page cachée : curseur « Maître » vers 1100, clic à la souris sur le deuxième onglet, 5 s, retour à la souris.
4. Tout arrêter (Espace), batterie débranchée.

- À l'étape 2, le moteur 1 n'a PAS changé de vitesse (curseur toujours vers 1100) ?
- À l'étape 3, les moteurs se sont arrêtés dès l'affichage de l'autre onglet, et au retour le test était sur non ?

## TG-B1-5. Retour en TRUEGLIDE cb4e3f0

1. Batterie débranchée. TRUEGLIDE /app/, onglet « Flasher le firmware » : SPEEDYBEEF405MINI, version en ligne
   **cb4e3f0**, **effacement complet**, flasher.
2. « Connexion », onglet CLI : coller tout le fichier `MINI_POUCE_TRUEGLIDE_diff_all_2026-11-08_cb4e3f0.txt`, Entrée.
   La carte redémarre.
3. Reconnecter, CLI : `version`, puis `get craft_name`.

- `version` contient cb4e3f0 et `craft_name` = MINI POUCE ?

## TG-B1-6. Baromètre : mesure AVANT correction (armé au banc, hélices retirées)

Ce qu'on mesure : combien d'erreurs I2C arrivent avec le firmware actuel, en 3 minutes. C'est la mesure de départ :
la même mesure sera refaite plus tard avec le firmware d'essai, pour comparer. Le Mini Pouce est en DShot par timer
(pas bitbang). Danger : carte **armée** au banc. Même hélices retirées, l'airmode et le terme I peuvent
faire monter les moteurs au-dessus de la commande : mains loin des moteurs, drone fixé, prise batterie à portée.

1. Onglet Blackbox : « Effacer la mémoire flash ».
2. CLI : `get dshot_bitbang` (noter la réponse), `dma show` (copier tout le texte affiché), puis
   `set debug_mode = BARO`, `save`.
3. Reconnecter, CLI : `status`, noter la ligne « I2C=… (… errors) ». **Garder le câble USB branché jusqu'à l'étape 6**
   (la carte reste alimentée : le compteur n'est pas remis à zéro), mais cliquer sur « Déconnecter » (armement
   impossible quand le logiciel est connecté).
4. Radio allumée. Vérifier d'abord que l'interrupteur d'armement désarme bien (armer 2 s, désarmer).
5. Batterie branchée, armer, gaz **juste au-dessus du ralenti**, **3 minutes**, sans à-coups. Désarmer. Débrancher la
   batterie (l'USB reste branché).
6. « Connexion », CLI : `status`, noter la ligne I2C. Onglet Blackbox : enregistrer le journal.

| Relevé | Attendu |
|---|---|
| Version de Betaflight App (partie 1) | à noter |
| `get dshot_bitbang` | OFF (configuration de la carte) |
| Ligne I2C avant (étape 3) | à noter |
| Ligne I2C après (étape 6) | à noter |

- La carte a désarmé du premier coup aux étapes 4 et 5 ?
- Le journal Blackbox est enregistré (nom du fichier noté) ?

## À envoyer

### Les vidéos TG-B1-2 à TG-B1-4 (noms des fichiers)

### Le journal Blackbox de TG-B1-6, les deux lignes I2C et le texte de `dma show` [grand]

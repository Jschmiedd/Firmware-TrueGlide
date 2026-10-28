# Essai TG-P2 (TRUEGLIDE 2026.6.2) : première carte, contrôle à plat

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), posé à plat sur la table, **HÉLICES RETIRÉES**. Logiciel : TrueGlide
Configurator (https://jschmiedd.github.io/TrueGlide-Configurator/app/), dans Chrome ou Edge sur ordinateur.
Matériel : batterie, multimètre, radio appairée, lunettes FPV, câble USB.

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices.
2. Dans Betaflight Configurator, carte branchée : faire la sauvegarde complète, comme d'habitude.
3. Dans TrueGlide Configurator, carte branchée, « Connexion », onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis « Enregistrer sous » (garder ce fichier).
4. Mesurer la tension de la batterie au multimètre et l'écrire dans le cadre du bas.
5. « Déconnecter ». Onglet « Flasher le Firmware », case « Mise à jour en ligne » : « Chercher la mise à jour », Modèle TRUEGLIDE, « Télécharger ». Cocher « Effacement complet de la puce », puis « Flasher le Firmware ». Ne pas débrancher pendant le flash.
6. Débrancher puis rebrancher l'USB, « Connexion ».
7. Remettre la configuration KarRmen (le flash a tout effacé) : onglet « Ligne de commande (CLI) », coller tout le texte du fichier de l'étape 3, Entrée, puis taper `save`, Entrée. La carte redémarre : « Connexion ». Si une ligne répond « ERROR », l'écrire dans « Autres remarques ».

## TG-P2-1. Flash et identité (onglet Tableau de bord)

- Le flash s'est terminé sans message d'erreur ?
- La case Firmware affiche « TRUEGLIDE 2026.6.2 (1791465) » ?
- La carte affichée est SPEEDYBEEF405MINI ?
- Case « Fonctions TrueGlide » : Protection anti-surchauffe, Relance moteur et Sorties commandées (PINIO) sur « Désactivé », Speeder à « 60° » ?

## TG-P2-2. Capteurs

Onglet « Ligne de commande (CLI) » : taper `status`, Entrée, et copier tout le texte dans le cadre du bas.

- La réponse de `status` contient « GYRO: (1) ICM42688P enabled », « ACC: ICM42688P » et « BARO: DPS310 » ?
- La réponse de `status` contient « GYRO rate: » suivi d'un nombre proche de 8000, et « I2C=1 (0 errors) » ?
- Onglet Installation, drone posé à plat : « Calibrer l'accéléromètre ». Ensuite, le modèle 3D est à plat ?
- En penchant le drone en roulis, en tangage, puis en le tournant (lacet), le modèle 3D suit dans le même sens ?

## TG-P2-3. Récepteur et modes (radio allumée)

- Onglet Récepteur : chaque voie (roulis, tangage, lacet, gaz) bouge dans le bon sens quand on bouge les manches ?
- Onglet Récepteur : les voies vont d'environ 1000 à 2000 ?
- Onglet Modes : chaque interrupteur (ARM, ANGLE…) allume le bon mode quand on le bascule ?
- Radio éteinte : le Tableau de bord affiche RX_FAILSAFE dans la case Drone ?

## TG-P2-4. Moteurs (HÉLICES RETIRÉES, batterie branchée)

Onglet « Ligne de commande (CLI) » : taper `get motor_pwm_protocol` puis `get dshot_bidir`.

- `motor_pwm_protocol` répond DSHOT300 ou DSHOT600 ?
- `dshot_bidir` répond ON ?
- Onglet Moteurs : cocher « Je comprends les risques », monter doucement le curseur de chaque moteur. La vitesse (tr/min) s'affiche sous le moteur qui tourne ?

## TG-P2-5. Moteurs, pour chacun (onglet Moteurs, curseur de ce moteur seulement, doucement)

- Moteur 1 : c'est le bon moteur qui tourne, et dans le bon sens ?
- Moteur 2 : c'est le bon moteur qui tourne, et dans le bon sens ?
- Moteur 3 : c'est le bon moteur qui tourne, et dans le bon sens ?
- Moteur 4 : c'est le bon moteur qui tourne, et dans le bon sens ?

## TG-P2-6. OSD et batterie (batterie branchée, lunettes allumées)

- L'OSD s'affiche dans les lunettes ?
- Tableau de bord : la tension de la batterie est la même qu'au multimètre, à 0,1 V près ?
- Onglet « Alimentation et batterie », moteurs arrêtés : le courant est proche de 0 A ?

## TG-P2-7. Fonctions TrueGlide (HÉLICES RETIRÉES)

On allume la protection, on vérifie, puis on la remet à 0 : à la fin, tout est comme juste après le flash.

- Onglet Protections, case « Protection anti-surchauffe / surintensité » : taper 80 dans « Température limite des ESC », puis « Enregistrer et redémarrer », puis « Connexion ». Le champ affiche toujours 80 ?
- Tableau de bord, case « Fonctions TrueGlide » : la ligne Protection anti-surchauffe affiche « Activé · 80 °C » ?
- Onglet Protections : remettre 0 dans « Température limite des ESC », « Enregistrer et redémarrer », « Connexion ». Le Tableau de bord affiche de nouveau « Désactivé » ?
- Onglet Modes : une ligne SPEEDER existe dans la liste ? (Ne pas l'assigner.)
- Onglet Caméra : la liste « Sortie commutable » propose PINIO 1 ? (Ne rien enregistrer.)

## TG-P2-8. Blackbox (HÉLICES RETIRÉES, batterie branchée, radio allumée)

- La réponse de `status` (partie 2) contient « FLASH: JEDEC ID=0x00ef4018 16M » ?
- Onglet Blackbox : « Effacer la mémoire interne », puis « Déconnecter » (le logiciel empêche l'armement tant qu'il est connecté). Armer avec la radio, attendre 10 secondes, désarmer, puis « Connexion ». L'onglet Blackbox montre que la mémoire n'est plus vide ?
- Onglet Blackbox : « Enregistrer sous… », puis ouvrir ce fichier dans l'onglet « Visualiseur Blackbox ». Le journal s'affiche ?

## TG-P2-9. Les réglages restent après coupure

Rien à régler ici : on vérifie que la carte garde tout ce qui a été fait pendant l'essai.

- Débrancher l'USB et la batterie, attendre 10 secondes, rebrancher l'USB, « Connexion ». Les interrupteurs de l'onglet Modes sont toujours là ?
- Onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

## À envoyer

### Tension de la batterie au multimètre

### Réponse de « status » (partie 2) [grand]

### Réponse de « diff all » (partie 9) [grand]

### Autres remarques (lignes « ERROR », ce qui a surpris)

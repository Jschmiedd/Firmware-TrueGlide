# Essai TG-P2 (TRUEGLIDE 2026.6.2) : première carte, contrôle à plat

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), posé à plat sur la table, **HÉLICES RETIRÉES**. Logiciel : TrueGlide
Configurator (https://jschmiedd.github.io/TrueGlide-Configurator/app/), dans Chrome ou Edge sur ordinateur.
Matériel : batterie, multimètre, radio appairée, lunettes FPV, câble USB. Les photos ont été prises en déroulant cette
fiche dans le même logiciel, avec le même firmware que celui de la carte (TRUEGLIDE 2026.6.2, révision 1791465) et la
configuration KarRmen : ce qu'il faut regarder est entouré en rouge.

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices.
2. Dans Betaflight Configurator, carte branchée : faire la sauvegarde complète, comme d'habitude.
3. Dans TrueGlide Configurator, carte branchée, « Connexion », onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis « Enregistrer sous » (garder ce fichier).
4. Mesurer la tension de la batterie au multimètre et l'écrire dans le cadre du bas.
5. « Déconnecter ». Onglet « Flasher le Firmware », case « Mise à jour en ligne » : « Chercher la mise à jour », Modèle TRUEGLIDE, « Télécharger ». Cocher « Effacement complet de la puce », puis « Flasher le Firmware ». Ne pas débrancher pendant le flash.
6. Débrancher puis rebrancher l'USB, « Connexion ». Une fenêtre « Danger » s'ouvre (l'accéléromètre n'est pas encore calibré) : cliquer « Fermer », il sera calibré à la partie 2. Si une fenêtre « L'application est prête à être utilisée hors ligne » s'ouvre : « OK ».
7. Remettre la configuration KarRmen (le flash a tout effacé) : onglet « Ligne de commande (CLI) », coller tout le texte du fichier de l'étape 3, Entrée, puis taper `save`, Entrée. La carte redémarre : « Connexion ». Si une ligne répond « ERROR », l'écrire dans « Autres remarques ».

![Étape 5 : onglet Flasher le Firmware, « Chercher la mise à jour » et « Effacement complet de la puce ».](photos/TG-P2/avant-5_flasher.jpg)

![Étape 6 : la fenêtre « Danger » après le flash ; cliquer « Fermer ».](photos/TG-P2/avant-6_danger.jpg)

![Étape 7 : configuration KarRmen collée dans le CLI (204 lignes, aucune ERROR pendant l'essai).](photos/TG-P2/avant-7_cli.jpg)

## TG-P2-1. Flash et identité (onglet Tableau de bord)

- Le flash s'est terminé sans message d'erreur ?
- La case Firmware affiche « TRUEGLIDE 2026.6.2 (1791465) » ?
- La carte affichée est SPEEDYBEEF405MINI ?
- Case « Fonctions TrueGlide » : Protection anti-surchauffe, Relance moteur et Sorties commandées (PINIO) sur « Désactivé », Speeder à « 60° » ?

![TG-P2-1.2 : case Firmware, « TRUEGLIDE 2026.6.2 (1791465) ».](photos/TG-P2/TG-P2-1.2.jpg)

![TG-P2-1.3 : SPEEDYBEEF405MINI, sous le titre Tableau de bord.](photos/TG-P2/TG-P2-1.3.jpg)

![TG-P2-1.4 : case Fonctions TrueGlide, tout sur « Désactivé », Speeder « 60° ».](photos/TG-P2/TG-P2-1.4.jpg)

## TG-P2-2. Capteurs

Onglet « Ligne de commande (CLI) » : taper `status`, Entrée, et copier tout le texte dans le cadre du bas.

- La réponse de `status` contient « GYRO: (1) ICM42688P enabled », « ACC: ICM42688P » et « BARO: DPS310 » ?
- La réponse de `status` contient « GYRO rate: » suivi d'un nombre proche de 8000, et « I2C=1 (0 errors) » ?
- Onglet Capteurs, case « Accéléromètre », drone posé à plat : cliquer « Calibrer ». Ensuite, le modèle 3D est à plat ?
- Onglet Installation : en penchant le drone en roulis, en tangage, puis en le tournant (lacet), le modèle 3D suit dans le même sens ?

![TG-P2-2.1 et 2.2 : réponse de status ; GYRO, ACC, BARO, puis GYRO rate et I2C.](photos/TG-P2/TG-P2-2.1.jpg)

![TG-P2-2.1 : les mêmes capteurs dans l'onglet Capteurs (Gyro 1, Accéléromètre, Baromètre).](photos/TG-P2/TG-P2-2.1b.jpg)

![TG-P2-2.3 : onglet Capteurs, case Accéléromètre, bouton « Calibrer ».](photos/TG-P2/TG-P2-2.3.jpg)

![TG-P2-2.4 : onglet Installation, le modèle 3D ; il bouge avec le vrai drone.](photos/TG-P2/TG-P2-2.4.jpg)

## TG-P2-3. Récepteur et modes (radio allumée)

- Onglet Récepteur : chaque barre (Roll, Pitch, Yaw, Throttle) bouge dans le bon sens quand on bouge les manches ?
- Onglet Récepteur : les barres vont d'environ 1000 à 2000 ?
- Onglet Modes : chaque interrupteur (ARM, ANGLE…) allume le bon mode quand on le bascule ?
- Radio éteinte : le Tableau de bord affiche RX_FAILSAFE dans la case Drone ?

![TG-P2-3.1 et 3.2 : onglet Récepteur, les barres des voies (ici à 1500, sans radio).](photos/TG-P2/TG-P2-3.1.jpg)

![TG-P2-3.3 : onglet Modes, la liste des modes.](photos/TG-P2/TG-P2-3.3.jpg)

![TG-P2-3.4 : Tableau de bord, case Drone, RX_FAILSAFE quand la radio est éteinte.](photos/TG-P2/TG-P2-3.4.jpg)

## TG-P2-4. Moteurs (HÉLICES RETIRÉES, batterie branchée)

- Onglet Moteurs, case « Fonctionnalités ESC/Moteur » : le protocole est DSHOT300 ou DSHOT600 ?
- Même case : l'interrupteur « DShot bi-directionnel » est activé ?
- Activer « Je comprends les risques », puis monter doucement le curseur de chaque moteur : la ligne « RPM » de ce moteur change quand il tourne ?

![TG-P2-4.1 : protocole DSHOT300, case Fonctionnalités ESC/Moteur.](photos/TG-P2/TG-P2-4.1m.jpg)

![TG-P2-4.2 : interrupteur « DShot bi-directionnel » activé.](photos/TG-P2/TG-P2-4.2m.jpg)

![TG-P2-4.3 : « Je comprends les risques » ; les curseurs 1 à 4 et les lignes RPM.](photos/TG-P2/TG-P2-4.3.jpg)

## TG-P2-5. Moteurs, pour chacun (onglet Moteurs, curseur de ce moteur seulement, doucement)

- Moteur 1 : c'est le bon moteur qui tourne, et dans le bon sens ?
- Moteur 2 : c'est le bon moteur qui tourne, et dans le bon sens ?
- Moteur 3 : c'est le bon moteur qui tourne, et dans le bon sens ?
- Moteur 4 : c'est le bon moteur qui tourne, et dans le bon sens ?

## TG-P2-6. OSD et batterie (batterie branchée, lunettes allumées)

- L'OSD s'affiche dans les lunettes ?
- Onglet « Alimentation et batterie », case « Etat alimentation » : la Tension est la même qu'au multimètre, à 0,1 V près ?
- Même case, moteurs arrêtés : l'Intensité est proche de 0 A ?

![TG-P2-6.2 : case Etat alimentation, ligne Tension (0,00 V ici : pas de batterie sur la carte simulée).](photos/TG-P2/TG-P2-6.2m.jpg)

![TG-P2-6.3 : case Etat alimentation, ligne Intensité.](photos/TG-P2/TG-P2-6.3.jpg)

## TG-P2-7. Fonctions TrueGlide (HÉLICES RETIRÉES)

On allume la protection, on vérifie, puis on la remet à 0 : à la fin, tout est comme juste après le flash.

- Onglet Protections, case « Protection anti-surchauffe / surintensité » : taper 80 dans « Température limite des ESC », puis « Enregistrer et redémarrer », puis « Connexion ». Le champ affiche toujours 80 ?
- Tableau de bord, case « Fonctions TrueGlide » : la ligne Protection anti-surchauffe affiche « Activé · 80 °C » ?
- Onglet Protections : remettre 0 dans « Température limite des ESC », « Enregistrer et redémarrer », « Connexion ». Le Tableau de bord affiche de nouveau « Désactivé » ?
- Onglet Modes : une ligne SPEEDER existe dans la liste ? (Ne pas l'assigner.)
- Onglet Caméra : cliquer sur la liste « Sortie commutable » (vide au départ) : elle propose « PINIO 1 (B11) » ? (Ne rien enregistrer.)

![TG-P2-7.1 : 80 tapé dans « Température limite des ESC ».](photos/TG-P2/TG-P2-7.1a.jpg)

![TG-P2-7.1 : après « Enregistrer et redémarrer » et « Connexion », le champ affiche toujours 80.](photos/TG-P2/TG-P2-7.1.jpg)

![TG-P2-7.2 : Tableau de bord, Protection anti-surchauffe « Activé · 80 °C ».](photos/TG-P2/TG-P2-7.2.jpg)

![TG-P2-7.3 : remis à 0, de nouveau « Désactivé ».](photos/TG-P2/TG-P2-7.3.jpg)

![TG-P2-7.4 : onglet Modes, la ligne SPEEDER.](photos/TG-P2/TG-P2-7.4.jpg)

![TG-P2-7.5 : onglet Caméra, la liste Sortie commutable ouverte, « PINIO 1 (B11) ».](photos/TG-P2/TG-P2-7.5.jpg)

## TG-P2-8. Blackbox (HÉLICES RETIRÉES, batterie branchée, radio allumée)

- La réponse de `status` (partie 2) contient « FLASH: JEDEC ID=0x00ef4018 16M » ?
- Onglet Blackbox : « Effacer la mémoire interne », puis « Déconnecter » (le logiciel empêche l'armement tant qu'il est connecté : motif MSP). Armer avec la radio, attendre 10 secondes, désarmer, puis « Connexion ». L'onglet Blackbox montre que la mémoire n'est plus vide ?
- Onglet Blackbox : « Enregistrer sous… », puis ouvrir ce fichier dans l'onglet « Visualiseur Blackbox ». Le journal s'affiche ?

![TG-P2-8.1 : réponse de status, ligne FLASH (16M).](photos/TG-P2/TG-P2-8.1.jpg)

![TG-P2-8.2 : onglet Blackbox, « Effacer la mémoire interne ».](photos/TG-P2/TG-P2-8.2.jpg)

![TG-P2-8.2 : pourquoi « Déconnecter » avant d'armer : motif MSP dans la case Drone.](photos/TG-P2/TG-P2-8.2b.jpg)

![TG-P2-8.3 : onglet Blackbox, « Enregistrer sous… ».](photos/TG-P2/TG-P2-8.3.jpg)

## TG-P2-9. Les réglages restent après coupure

Rien à régler ici : on vérifie que la carte garde tout ce qui a été fait pendant l'essai.

- Débrancher l'USB et la batterie, attendre 10 secondes, rebrancher l'USB, « Connexion ». Les interrupteurs de l'onglet Modes sont toujours là ?
- Onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

![TG-P2-9.2 : réponse de diff all dans le CLI.](photos/TG-P2/TG-P2-9.2.jpg)

## À envoyer

### Tension de la batterie au multimètre

### Réponse de « status » (partie 2) [grand]

### Réponse de « diff all » (partie 9) [grand]

### Autres remarques (lignes « ERROR », ce qui a surpris)

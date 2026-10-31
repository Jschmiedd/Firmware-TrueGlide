# Essai TG-P3B (TRUEGLIDE 2026.6.2, révision 24a71eb) : fonctions TrueGlide en vol

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), configuration de TG-P3 (identique à Betaflight). Logiciel : TrueGlide
Configurator, onglet « Ligne de commande (CLI) ». Les fonctions sont essayées **une par une**, puis remises à zéro :
à la fin, tout est comme au départ. Vol en extérieur, lieu dégagé, personne autour, batterie pleine à chaque partie.
**Pas d'ALTHOLD ni de POS HOLD.** Hélices bien centrées sur l'axe (voir TG-P3).

## Avant de commencer (rien à cocher)

1. TG-P3 faite (vols en ANGLE et en ACRO comme sous Betaflight) et TG-P3A faite le 30/10 (les mêmes fonctions au banc,
   sans hélices : tout validé). Radio : EdgeTX 2.11.8, ExpressLRS 4.1.0 (radio et récepteur du KarRmen).
   Firmware 24a71eb flashé le 31/10 et baromètre contrôlé au banc (TG-P2B, TG-P2C) : rien à reflasher.
   Tableau de bord, case Firmware : « TRUEGLIDE 2026.6.2 (24a71eb) ».
2. Batterie pleine (environ 16,8 V). Onglet Blackbox : « Effacer la mémoire interne ».
3. Après chaque partie : batterie rechargée ou changée, et noter dans « Autres remarques » ce qui a surpris.

## TG-P3B-1. Limite de courant (protection anti-surchauffe / surintensité)

En vol stationnaire, le drone consomme environ 8 A ; en montée franche, plus de 50 A. On met une limite à 20 A : en
montée franche, les gaz doivent être limités (jamais coupés) et l'OSD doit afficher « AMP LIM ».

1. CLI : taper `set power_guard_current = 20`, Entrée, puis `save`, Entrée. « Connexion ».
2. Vol en ANGLE, à 3 m de haut au moins : 3 montées franches, gaz à fond pendant 1 seconde chacune.
3. Atterrir, désarmer. CLI : taper `set power_guard_current = 0`, Entrée, puis `save`, Entrée. « Connexion ».

- Pendant les montées, l'OSD affiche « AMP LIM » suivi d'un pourcentage ?
- Pendant les montées, le drone monte moins fort que d'habitude, mais les moteurs ne se coupent jamais ?
- Après l'étape 3, Tableau de bord, case « Fonctions TrueGlide » : Protection anti-surchauffe sur « Désactivé » ?

## TG-P3B-2. Limite de température des ESC

La carte ne reçoit la température des ESC qu'avec `dshot_edt = ON` et après un armement (vérifié au banc, TG-P3A-2).
Au banc, les ESC étaient entre 28 et 42 °C. Avant le vol, on lit la température dans l'OSD, puis on met la limite
10 °C au-dessus de la plus haute : quand un ESC la dépasse, les gaz sont limités (au moins 50 %, jamais coupés) et l'OSD
affiche « ESC HOT ».

1. CLI : taper `set dshot_edt = ON`, Entrée, puis `save`, Entrée. Débrancher l'USB.
2. Brancher la batterie, lunettes allumées, armer au sol 3 secondes, désarmer : noter la température des ESC lue dans
   l'OSD. Débrancher la batterie.
3. Brancher l'USB, « Connexion ». CLI : taper `set power_guard_esc_temp = ` suivi de la température notée + 10 (exemple :
   35 °C lus → `set power_guard_esc_temp = 45`), Entrée, puis `save`, Entrée. Débrancher l'USB.
4. Vol habituel en ACRO, 2 minutes, en restant près de soi.
5. Atterrir, désarmer. Brancher l'USB, « Connexion ». CLI : taper `set power_guard_esc_temp = 0`, Entrée, puis
   `set dshot_edt = OFF`, Entrée, puis `save`, Entrée. « Connexion ».

- Avant le vol (étape 2), l'OSD affiche une température des ESC (pas 0 °C) ?
- Pendant le vol, l'OSD a affiché « ESC HOT » suivi d'un pourcentage ? (Si la température n'a jamais atteint la limite,
  répondre « non » et noter dans « Autres remarques » la température la plus haute vue : ce n'est pas une panne.)
- Quand « ESC HOT » était affiché, le drone volait toujours, les moteurs ne se sont jamais coupés ?
- Après l'étape 5, Tableau de bord : Protection anti-surchauffe sur « Désactivé » ?

## TG-P3B-3. Relance d'un moteur calé

On vérifie seulement que la fonction ne se déclenche pas à tort en vol normal (un vrai moteur calé ne s'essaie pas).

1. CLI : taper `set motor_desync_recovery = ON`, Entrée, puis `save`, Entrée. « Connexion ».
2. Vol habituel en ACRO, 2 minutes.
3. Atterrir, désarmer. CLI : taper `set motor_desync_recovery = OFF`, Entrée, puis `save`, Entrée. « Connexion ».

- Pendant le vol, l'OSD n'a jamais affiché « M1 RESTART », « M2 RESTART », « M3 RESTART » ni « M4 RESTART » ?
- Le drone a volé comme d'habitude ?
- Après l'étape 3, Tableau de bord : Relance moteur sur « Désactivé » ?

## TG-P3B-4. Température des ESC sur la radio, en vol

Validé au banc le 30/10 (TG-P3A-4) : radio EdgeTX 2.11.8 et ExpressLRS 4.1.0, 4 capteurs « Temp » (un par moteur) déjà
enregistrés dans le modèle. Ici on regarde les valeurs monter en vol.

1. CLI : taper `set dshot_edt = ON`, Entrée, puis `set crsf_tlm_esc_temp = ON`, Entrée, puis `save`, Entrée. Débrancher
   l'USB.
2. Vol habituel, 2 minutes (peut être le même vol que la partie 3).
3. Après l'atterrissage, désarmer sans débrancher la batterie. Radio : « MDL », page « TELEMETRY » : lire les 4 « Temp ».
4. Débrancher la batterie, brancher l'USB, « Connexion ». CLI : taper `set crsf_tlm_esc_temp = OFF`, Entrée, puis
   `set dshot_edt = OFF`, Entrée, puis `save`, Entrée.

- À l'étape 3, les 4 « Temp » de la radio ont une valeur, plus haute qu'avant le vol ? (Noter les 4 valeurs.)

## TG-P3B-5. Speeder (inclinaison automatique vers l'avant)

Le Speeder penche le drone vers l'avant d'un angle fixe, progressivement, quand son interrupteur est activé, en ANGLE
seulement. Premier essai avec un petit angle : 15°.

1. CLI : taper `set speeder_cruise_angle = 15`, Entrée, puis `save`, Entrée. « Connexion ».
2. Mettre SPEEDER sur l'interrupteur d'ALTHOLD (AUX4), par le CLI (l'onglet Modes déplace les plages par crans de 25) :
   taper `aux 3 120 3 1900 2100 0 0`, Entrée, puis `save`, Entrée.
3. Vol en ANGLE, stationnaire à 3 m, avec de la place devant (au moins 30 m). Basculer l'interrupteur SPEEDER pendant
   3 secondes, puis le remettre. Recommencer une fois.
4. Atterrir, désarmer. CLI : taper `aux 3 3 3 1900 2100 0 0`, Entrée (ALTHOLD revient sur AUX4), puis
   `set speeder_cruise_angle = 60`, Entrée, puis `save`, Entrée. Ne pas utiliser l'onglet Modes.

- Interrupteur SPEEDER activé : le drone penche doucement vers l'avant (en une demi-seconde environ) et avance ?
- Interrupteur remis : le drone revient doucement à plat ?
- Pendant l'essai, le drone est resté stable (pas d'oscillation, pas de perte de hauteur brusque) ?
- Après l'étape 4, `diff all` : la ligne `aux 3 3 3 1900 2100 0 0` est revenue et aucune ligne `speeder` ?

## TG-P3B-6. Après les essais (USB branché, batterie débranchée)

- Onglet Blackbox : « Enregistrer sous… » a créé le fichier du journal (à envoyer) ?
- Tableau de bord, case « Fonctions TrueGlide » : tout sur « Désactivé » et Speeder à « 60° » ?
- Onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

## À envoyer

### Fichiers envoyés (journal Blackbox, fichiers .osd des vols, diff all)

### Réponse de « diff all » (partie 6) [grand]

### Autres remarques (ce qui a surpris, températures vues dans l'onglet Moteurs)

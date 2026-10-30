# Essai TG-P3B (TRUEGLIDE 2026.6.2, révision f9fc6d5) : fonctions TrueGlide en vol

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), configuration de TG-P3 (identique à Betaflight). Logiciel : TrueGlide
Configurator, onglet « Ligne de commande (CLI) ». Les fonctions sont essayées **une par une**, puis remises à zéro :
à la fin, tout est comme au départ. Vol en extérieur, lieu dégagé, personne autour, batterie pleine à chaque partie.
**Pas d'ALTHOLD ni de POS HOLD.** Hélices bien centrées sur l'axe (voir TG-P3).

## Avant de commencer (rien à cocher)

1. TG-P3 faite (vols en ANGLE et en ACRO comme sous Betaflight) et TG-P3A faite (les mêmes fonctions au banc, sans hélices).
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

Au repos, les ESC sont vers 25 à 35 °C (onglet Moteurs, ligne « Temp »). On met une limite basse, 40 °C, pour la voir
agir : quand un ESC dépasse 40 °C, les gaz sont limités (au moins 50 %, jamais coupés) et l'OSD affiche « ESC HOT ».

1. CLI : taper `set dshot_edt = ON`, Entrée, puis `set power_guard_esc_temp = 40`, Entrée, puis `save`, Entrée. « Connexion ».
2. Vol habituel en ACRO, 2 minutes, en restant près de soi.
3. Atterrir, désarmer. CLI : taper `set power_guard_esc_temp = 0`, Entrée, puis `set dshot_edt = OFF`, Entrée, puis
   `save`, Entrée. « Connexion ».

- Pendant le vol, l'OSD a affiché « ESC HOT » suivi d'un pourcentage ? (Si la température n'a jamais atteint 40 °C,
  répondre « non » et l'écrire dans « Autres remarques » : ce n'est pas une panne.)
- Quand « ESC HOT » était affiché, le drone volait toujours, les moteurs ne se sont jamais coupés ?
- Après l'étape 3, Tableau de bord : Protection anti-surchauffe sur « Désactivé » ?

## TG-P3B-3. Relance d'un moteur calé

On vérifie seulement que la fonction ne se déclenche pas à tort en vol normal (un vrai moteur calé ne s'essaie pas).

1. CLI : taper `set motor_desync_recovery = ON`, Entrée, puis `save`, Entrée. « Connexion ».
2. Vol habituel en ACRO, 2 minutes.
3. Atterrir, désarmer. CLI : taper `set motor_desync_recovery = OFF`, Entrée, puis `save`, Entrée. « Connexion ».

- Pendant le vol, l'OSD n'a jamais affiché « M1 RESTART », « M2 RESTART », « M3 RESTART » ni « M4 RESTART » ?
- Le drone a volé comme d'habitude ?
- Après l'étape 3, Tableau de bord : Relance moteur sur « Désactivé » ?

## TG-P3B-4. Température des ESC envoyée à la radio

Au sol, hélices retirées, batterie branchée, radio allumée.

1. CLI : taper `set dshot_edt = ON`, Entrée, puis `set crsf_tlm_esc_temp = ON`, Entrée, puis `save`, Entrée. « Connexion ».
   Débrancher l'USB, brancher la batterie, armer 3 secondes gaz en bas, désarmer.
2. Radio : page « Télémesure » du modèle, « Découvrir de nouveaux capteurs », attendre 10 secondes, « Arrêter ».
3. Débrancher la batterie, brancher l'USB, « Connexion ». CLI : taper `set crsf_tlm_esc_temp = OFF`, Entrée, puis
   `set dshot_edt = OFF`, Entrée, puis `save`, Entrée. « Connexion ».

- À l'étape 2, un nouveau capteur de température apparaît dans la liste de la radio, avec une valeur proche de celle
  de l'onglet Moteurs (ligne « Temp ») ?

## TG-P3B-5. Speeder (inclinaison automatique vers l'avant)

Le Speeder penche le drone vers l'avant d'un angle fixe, progressivement, quand son interrupteur est activé, en ANGLE
seulement. Premier essai avec un petit angle : 15°.

1. CLI : taper `set speeder_cruise_angle = 15`, Entrée, puis `save`, Entrée. « Connexion ».
2. Onglet Modes : sur la ligne SPEEDER, « Ajouter une plage » sur un interrupteur libre de la radio. S'il n'y en a pas,
   prendre celui d'ALTHOLD (AUX4) : supprimer la plage d'ALTHOLD et mettre SPEEDER sur AUX4, 1900 à 2100. « Enregistrer ».
3. Vol en ANGLE, stationnaire à 3 m, avec de la place devant (au moins 30 m). Basculer l'interrupteur SPEEDER pendant
   3 secondes, puis le remettre. Recommencer une fois.
4. Atterrir, désarmer. Onglet Modes : supprimer la plage SPEEDER (et remettre ALTHOLD sur AUX4, 1900 à 2100, si on
   l'avait enlevé). « Enregistrer ». CLI : taper `set speeder_cruise_angle = 60`, Entrée, puis `save`, Entrée.

- Interrupteur SPEEDER activé : le drone penche doucement vers l'avant (en une demi-seconde environ) et avance ?
- Interrupteur remis : le drone revient doucement à plat ?
- Pendant l'essai, le drone est resté stable (pas d'oscillation, pas de perte de hauteur brusque) ?
- Après l'étape 4, onglet Modes : SPEEDER sans plage, et ALTHOLD comme avant ?

## TG-P3B-6. Après les essais (USB branché, batterie débranchée)

- Onglet Blackbox : « Enregistrer sous… » a créé le fichier du journal (à envoyer) ?
- Tableau de bord, case « Fonctions TrueGlide » : tout sur « Désactivé » et Speeder à « 60° » ?
- Onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

## À envoyer

### Fichiers envoyés (journal Blackbox, fichiers .osd des vols, diff all)

### Réponse de « diff all » (partie 6) [grand]

### Autres remarques (ce qui a surpris, températures vues dans l'onglet Moteurs)

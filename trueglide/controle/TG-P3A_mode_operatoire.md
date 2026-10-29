# Essai TG-P3A (TRUEGLIDE 2026.6.2, révision f9fc6d5) : fonctions TrueGlide au banc, sans hélices

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), **HÉLICES RETIRÉES**, posé à plat sur la table et tenu (il ne peut pas
s'envoler, mais il peut glisser). Configuration de TG-P3 (identique à Betaflight). Matériel : batterie, radio,
lunettes FPV, câble USB. Les fonctions sont essayées **une par une**, puis remises à zéro : à la fin, tout est comme
au départ. Cette fiche se fait **avant** TG-P3B (les mêmes fonctions en vol).

Pour armer, le logiciel doit être déconnecté (motif MSP) : pour chaque partie, on règle dans le CLI, on déconnecte, on
débranche l'USB, on branche la batterie, on arme avec la radio, on regarde l'OSD dans les lunettes, on désarme, puis on
débranche la batterie et on rebranche l'USB pour remettre le réglage à zéro.

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices.
2. Onglet Blackbox : « Effacer la mémoire interne ».
3. Onglet Moteurs : noter dans « Autres remarques » les températures des 4 ESC au repos (ligne « Temp »), batterie
   branchée. Débrancher la batterie.

## TG-P3A-1. Limite de courant

Sans hélices, les moteurs consomment peu (quelques ampères à fond). On met une limite très basse, 2 A, pour la voir agir.

1. CLI : taper `set power_guard_current = 2`, Entrée, puis `save`, Entrée. « Déconnecter », débrancher l'USB.
2. Brancher la batterie, lunettes allumées. Armer, monter doucement les gaz jusqu'en haut, rester 3 secondes, redescendre,
   désarmer. Débrancher la batterie.
3. Brancher l'USB, « Connexion ». CLI : taper `set power_guard_current = 0`, Entrée, puis `save`, Entrée. « Connexion ».

- Gaz en haut : l'OSD affiche « AMP LIM » suivi d'un pourcentage ? (Si l'intensité de l'OSD n'a jamais dépassé 2 A,
  répondre « non » et noter l'intensité maximale vue dans « Autres remarques ».)
- Pendant « AMP LIM », les moteurs tournent toujours (ils ne se coupent pas) ?
- Après l'étape 3, Tableau de bord, case « Fonctions TrueGlide » : Protection anti-surchauffe sur « Désactivé » ?

## TG-P3A-2. Limite de température des ESC

On met une limite **sous** la température la plus haute notée au repos (par exemple 30 °C si un ESC est à 34 °C) : la
protection doit agir dès l'armement.

1. CLI : taper `set power_guard_esc_temp = 30`, Entrée (ou la valeur choisie), puis `save`, Entrée. « Déconnecter »,
   débrancher l'USB.
2. Brancher la batterie, lunettes allumées. Armer, gaz à mi-course pendant 5 secondes, désarmer. Débrancher la batterie.
3. Brancher l'USB, « Connexion ». CLI : taper `set power_guard_esc_temp = 0`, Entrée, puis `save`, Entrée. « Connexion ».

- Armé : l'OSD affiche « ESC HOT » suivi d'un pourcentage ?
- Pendant « ESC HOT », les moteurs tournent toujours (ils ne se coupent pas) ?
- Après l'étape 3, Tableau de bord : Protection anti-surchauffe sur « Désactivé » ?

## TG-P3A-3. Relance d'un moteur calé

On vérifie qu'elle ne se déclenche pas à tort quand les moteurs tournent normalement.

1. CLI : taper `set motor_desync_recovery = ON`, Entrée, puis `save`, Entrée. « Déconnecter », débrancher l'USB.
2. Brancher la batterie, lunettes allumées. Armer, puis pendant 30 secondes monter et descendre les gaz, doucement puis
   vite, plusieurs fois. Désarmer. Débrancher la batterie.
3. Brancher l'USB, « Connexion ». CLI : taper `set motor_desync_recovery = OFF`, Entrée, puis `save`, Entrée. « Connexion ».

- Pendant les 30 secondes, l'OSD n'a jamais affiché « M1 RESTART », « M2 RESTART », « M3 RESTART » ni « M4 RESTART » ?
- Après l'étape 3, Tableau de bord : Relance moteur sur « Désactivé » ?

## TG-P3A-4. Température des ESC envoyée à la radio

1. CLI : taper `set crsf_tlm_esc_temp = ON`, Entrée, puis `save`, Entrée. « Déconnecter », débrancher l'USB.
2. Brancher la batterie, radio allumée. Radio : page « Télémesure » du modèle, « Découvrir de nouveaux capteurs »,
   attendre 10 secondes, « Arrêter ». Débrancher la batterie.
3. Brancher l'USB, « Connexion ». CLI : taper `set crsf_tlm_esc_temp = OFF`, Entrée, puis `save`, Entrée. « Connexion ».

- À l'étape 2, un nouveau capteur de température apparaît dans la liste de la radio, avec une valeur proche des
  températures notées au début ?

## TG-P3A-5. Speeder

Au banc, le drone ne peut pas pencher : quand le Speeder s'active, il « veut » pencher vers l'avant, donc les moteurs
arrière (1 et 3) accélèrent et les moteurs avant (2 et 4) ralentissent. Le journal Blackbox l'enregistre.

1. CLI : taper `set speeder_cruise_angle = 15`, Entrée, puis `save`, Entrée. « Connexion ».
2. Onglet Modes : sur la ligne SPEEDER, « Ajouter une plage » sur un interrupteur libre de la radio. S'il n'y en a pas,
   prendre celui d'ALTHOLD (AUX4) : supprimer la plage d'ALTHOLD et mettre SPEEDER sur AUX4, 1900 à 2100. « Enregistrer ».
   « Déconnecter », débrancher l'USB.
3. Brancher la batterie. Interrupteur des modes sur ANGLE. Armer, gaz bas (moteurs au ralenti). Basculer l'interrupteur
   SPEEDER pendant 3 secondes, puis le remettre. Recommencer une fois. Désarmer. Débrancher la batterie.
4. Brancher l'USB, « Connexion ». Onglet Modes : supprimer la plage SPEEDER (et remettre ALTHOLD sur AUX4, 1900 à 2100, si
   on l'avait enlevé). « Enregistrer ». CLI : taper `set speeder_cruise_angle = 60`, Entrée, puis `save`, Entrée.

- À l'étape 3, en basculant SPEEDER, le son des moteurs change (les moteurs arrière accélèrent) ?
- Après l'étape 4, onglet Modes : SPEEDER sans plage, et ALTHOLD comme avant ?

## TG-P3A-6. Fin (USB branché, batterie débranchée)

- Onglet Blackbox : « Enregistrer sous… » a créé le fichier du journal (à envoyer) ?
- Tableau de bord, case « Fonctions TrueGlide » : tout sur « Désactivé » et Speeder à « 60° » ?
- Onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

## À envoyer

### Fichiers envoyés (journal Blackbox, fichiers .osd, diff all)

### Réponse de « diff all » (partie 6) [grand]

### Autres remarques (températures des ESC au repos, intensité maximale vue, ce qui a surpris)

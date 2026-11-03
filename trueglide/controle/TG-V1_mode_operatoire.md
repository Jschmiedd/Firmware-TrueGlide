# Essai TG-V1 (TRUEGLIDE 2026.6.2, révision 24a71eb) : premier vol en intérieur, fonctions TrueGlide coupées

Date : …… Drone : …… Fait par : ……

Drone : KarRmen / Mini Pouce (SpeedyBee F405 Mini), avec **ta configuration du 31/10** (celle qui volait sous Betaflight) :
fonctions TrueGlide **coupées**. But : vérifier que le drone vole **comme sous Betaflight** avec le firmware TRUEGLIDE
24a71eb. Lieu : grande salle (gymnase, hangar), **personne à moins de 5 m**. Matériel : batterie 4S chargée, radio,
lunettes FPV, câble USB. Vol court : 1 à 2 minutes. Au moindre comportement anormal (oscillation, bruit aigu, dérive
forte, réaction bizarre aux manches) : **atterrir et désarmer tout de suite**, puis noter ce qui s'est passé.

## Avant de commencer (rien à cocher)

1. **Hélices retirées**, USB branché, batterie débranchée. « Connexion ».
2. Onglet Blackbox : « Effacer la mémoire interne », attendre la fin.
3. Onglet « Ligne de commande (CLI) » : taper `status`, Entrée. Copier toute la réponse dans le cadre « status (avant) » du bas.

## TG-V1-1. Contrôle au sol (USB branché, hélices retirées)

1. Tableau de bord, case « Fonctions TrueGlide » : lire les lignes.
2. Onglet Moteurs : brancher la batterie, cocher « Je comprends les risques », monter chaque moteur seul à environ 10 %,
   une seconde, puis le remettre à zéro. Débrancher la batterie.

- Protection anti-surchauffe et Relance moteur sur « Désactivé » ?
- Les 4 moteurs tournent, chacun dans le sens de son schéma (onglet Moteurs) ?
- Aucun bruit ni vibration anormale moteur par moteur ?

## TG-V1-2. Mise en place (sans USB)

1. « Déconnecter », débrancher l'USB.
2. Monter les 4 hélices dans le bon sens (comme pour tes vols sous Betaflight), bien serrées.
3. Poser le drone au centre de la salle, à plat. Radio allumée, mode de vol habituel (ANGLE conseillé), gaz en bas.
4. Brancher la batterie, lunettes allumées, attendre les bips de démarrage. Ne pas bouger le drone pendant 5 secondes
   (étalonnage du gyroscope).

- Dans les lunettes, l'OSD s'affiche (tension, minuteur), sans alerte inattendue ?

## TG-V1-3. Vol (1 à 2 minutes)

1. Armer. Monter les gaz doucement jusqu'à décoller, puis rester à environ 1 m de haut.
2. Stationnaire 30 secondes, sans toucher aux manches de direction.
3. Petits déplacements : avant, arrière, gauche, droite, puis un lent tour sur place (lacet).
4. Atterrir doucement, désarmer, débrancher la batterie.

- Le drone s'arme normalement ?
- En stationnaire, il reste stable, sans oscillation ni bruit aigu ?
- Il répond aux manches comme sous Betaflight ?
- Pendant le vol, l'OSD n'affiche aucune alerte TrueGlide (« BAT RTH », « AMP LIM », « ESC HOT », « RESTART ») ?
- Après l'atterrissage, batterie débranchée : les moteurs sont tièdes, pas chauds au toucher ?

## TG-V1-4. Relevé (USB branché, batterie débranchée)

1. Brancher l'USB, « Connexion ».
2. Onglet Blackbox : « Enregistrer sous… ».
3. Onglet « Ligne de commande (CLI) » : taper `status`, Entrée. Copier toute la réponse dans le cadre « status (après) » du bas.

- Le fichier du journal Blackbox est enregistré ?
- Dans la réponse de `status`, le compteur d'erreurs I2C est à 0 ?

## À envoyer

### Fichier du journal Blackbox : nom du fichier envoyé

### Réponse de « status » (avant) [grand]

### Réponse de « status » (après) [grand]

### Ressenti en vol et autres remarques (comparaison avec Betaflight, ce qui a surpris) [grand]

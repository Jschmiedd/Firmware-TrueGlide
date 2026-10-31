# Essai TG-P2C (TRUEGLIDE 2026.6.2, révision 24a71eb) : baromètre avec la bande LED

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), posé à plat, **HÉLICES RETIRÉES**. Logiciel : TrueGlide Configurator, dans
Chrome ou Edge. But : refaire la partie armée de TG-P2B **avec la bande LED activée** (c'est avec elle que le baromètre
déraillait les 28 et 29 octobre). Une seule fois : si les deux contrôles ✅ sont bons, l'essai est valable quel que soit
le résultat.

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices. Batterie débranchée, USB branché.
2. « Connexion ». Onglet « Ligne de commande (CLI) » : taper chaque ligne puis Entrée : `feature LED_STRIP`, puis
   `set debug_mode = BARO`, puis `save`. La carte redémarre : « Connexion ».

## TG-P2C-1. Réglage (USB branché, batterie débranchée)

1. Onglet « Ligne de commande (CLI) » : taper `feature`, Entrée.
2. Onglet Blackbox : « Effacer la mémoire interne », attendre la fin.
3. « Déconnecter », puis débrancher l'USB.

- ✅ À l'étape 1, la réponse de `feature` contient le mot **LED_STRIP** ? (sinon : arrêter ici et envoyer une capture)
- À l'étape 2, la mémoire est vide après l'effacement ?

## TG-P2C-2. Essai armé (HÉLICES RETIRÉES, sans USB)

1. Brancher la batterie, allumer les lunettes.
2. Armer, compter **30 secondes** sur le minuteur de l'OSD en regardant l'altitude, ne pas toucher aux gaz. Désarmer.

- Pendant les 30 secondes, l'altitude de l'OSD reste entre −10 et +10 m, sans saut ?

## TG-P2C-3. Relevé (NE PAS DÉBRANCHER LA BATTERIE)

1. Batterie toujours branchée : brancher l'USB, « Connexion ».
2. Onglet « Ligne de commande (CLI) » : taper `status`, Entrée. Copier toute la réponse dans le cadre du bas.
3. Onglet Blackbox : « Enregistrer sous… ».
4. Onglet « Ligne de commande (CLI) » : taper chaque ligne puis Entrée : `feature -LED_STRIP`, puis
   `set debug_mode = NONE`, puis `save`.
5. Débrancher la batterie.

- ✅ À l'étape 2, « System Uptime » est supérieur à 30 secondes ?
- À l'étape 3, le fichier du journal Blackbox est enregistré ?

## À envoyer

### Fichier du journal Blackbox : nom du fichier envoyé

### Réponse de « status » (étape 3.2) [grand]

### Autres remarques

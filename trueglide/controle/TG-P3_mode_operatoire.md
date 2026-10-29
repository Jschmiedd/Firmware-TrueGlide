# Essai TG-P3 (TRUEGLIDE 2026.6.2, révision f9fc6d5) : premiers vols

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), la même configuration que sous Betaflight. Logiciel : TrueGlide Configurator
(https://jschmiedd.github.io/TrueGlide-Configurator/app/), dans Chrome ou Edge sur ordinateur. Matériel : batterie
chargée, radio, lunettes FPV, câble USB, hélices. Toutes les fonctions TrueGlide restent désactivées : le drone doit
voler exactement comme sous Betaflight. **Pas d'ALTHOLD ni de POS HOLD dans cette fiche.** Le failsafe a déjà été
contrôlé (TG-P2-3.4) : il n'est pas refait.

## Avant de commencer (rien à cocher)

1. **La fiche TG-P2B (baromètre) est faite et validée** : carte en TRUEGLIDE révision f9fc6d5, altitude stable, journal envoyé. Sinon, la faire d'abord.
2. Hélices retirées, USB branché, « Connexion ». Onglet « Ligne de commande (CLI) » : taper `feature -LED_STRIP`, Entrée (comme sous Betaflight), puis `save`, Entrée. La carte redémarre : « Connexion ».
3. Onglet Capteurs, case « Accéléromètre », drone posé à plat : « Calibrer ».
4. Batterie chargée (4S pleine : environ 16,8 V au multimètre) ; écrire sa tension dans le cadre du bas. Vol dans un lieu dégagé, personne autour.

## TG-P3-1. Firmware (onglet Tableau de bord)

- La case Firmware affiche « TRUEGLIDE 2026.6.2 (f9fc6d5) » ?
- Case « Fonctions TrueGlide » : Protection anti-surchauffe, Relance moteur et Sorties commandées (PINIO) sur « Désactivé » ?

## TG-P3-2. Préparation du vol

1. Onglet Blackbox : « Effacer la mémoire interne ». Puis « Déconnecter » et débrancher l'USB.
2. Monter les 4 hélices, dans le même sens que sous Betaflight.
3. Radio : interrupteur ARM sur arrêt, interrupteur des modes sur ANGLE.

- Les 4 hélices sont montées dans le bon sens et serrées ?
- Batterie branchée, OSD dans les lunettes : la tension affichée est celle d'une batterie pleine (environ 16,8 V) ?

## TG-P3-3. Premier vol en ANGLE (stationnaire, 1 à 2 m de haut, 1 minute)

- Le drone s'arme normalement et décolle droit, sans partir d'un côté ?
- En stationnaire : aucune oscillation, aucune vibration ni bruit moteur anormal ?
- Les commandes répondent comme sous Betaflight (même sens, même douceur) ?
- OSD : l'altitude suit la hauteur réelle (quelques mètres), sans saut ?
- Après l'atterrissage et le désarmement : les 4 moteurs sont tièdes au toucher (on peut garder le doigt posé dessus) ?

## TG-P3-4. Vol en ACRO (vol habituel, 2 à 3 minutes)

- Le drone vole comme sous Betaflight : pas d'oscillation dans les virages, les montées et les descentes ?
- OSD : aucun message d'alerte inattendu (seule l'alerte de batterie basse est normale en fin de vol) ?
- OSD : l'altitude reste cohérente avec le vol, sans saut de centaines ou de milliers de mètres ?
- Après l'atterrissage et le désarmement : les 4 moteurs sont tièdes au toucher ?
- Tension à l'atterrissage au-dessus de 14,0 V (3,5 V par élément) ?

## TG-P3-5. Après les vols (USB branché, batterie débranchée)

- Onglet Blackbox : la mémoire n'est plus vide, et « Enregistrer sous… » a créé le fichier du journal ?
- Onglet « Visualiseur Blackbox » : le journal des vols s'ouvre et s'affiche ?
- Onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

## À envoyer

### Tension de la batterie avant le vol (multimètre)

### Fichiers envoyés (journal des vols, fichier .osd du vol, diff all)

### Réponse de « diff all » (partie 5) [grand]

### Autres remarques (comportement en vol, ce qui a surpris)

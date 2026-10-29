# Essai TG-P3 (TRUEGLIDE 2026.6.2, révision f9fc6d5) : premiers vols

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), la même configuration que sous Betaflight. Logiciel : TrueGlide Configurator
(https://jschmiedd.github.io/TrueGlide-Configurator/app/), dans Chrome ou Edge sur ordinateur. Matériel : batterie
chargée, radio, lunettes FPV, câble USB, hélices. Toutes les fonctions TrueGlide restent désactivées : le drone doit
voler exactement comme sous Betaflight. **Pas d'ALTHOLD ni de POS HOLD dans cette fiche** (le baromètre est d'abord
vérifié à la partie 2). Le failsafe a déjà été contrôlé (TG-P2-3.4) : il n'est pas refait. Les photos ont été prises
en déroulant cette fiche dans le même logiciel, avec le même firmware (révision f9fc6d5, carte simulée) et la
configuration KarRmen : ce qu'il faut regarder est entouré en rouge. Ce qui se passe en vol ne se photographie pas
en simulation : ces questions n'ont pas de photo.

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices.
2. Flasher le fichier trueglide_2026.6.2_STM32F405_SPEEDYBEEF405MINI.hex de la révision f9fc6d5 (envoyé le 29 octobre) : Betaflight Configurator, onglet « Flasher le Firmware », menu à côté de « Charger un Firmware [Online] » : chargement **local**, choisir ce fichier, cocher « Effacement complet de la puce », « Flasher le Firmware », attendre la fin.
   - Ne pas prendre la mise à jour en ligne : elle installe la révision 1791465, dont le baromètre donne des mesures fausses.
   - Ne pas débrancher pendant le flash.
3. Fermer Betaflight Configurator. Ouvrir TrueGlide Configurator, « Connexion ». La fenêtre « Danger » s'ouvre (l'accéléromètre n'est pas encore calibré) : « Fermer ».
4. Onglet « Ligne de commande (CLI) » : ouvrir le fichier `diff all` de Betaflight du 14 octobre (1505_DP_4000kv_BTFL_cli_KARRMEN_20261014_110629), copier tout son texte, le coller dans la ligne du bas, Entrée. Attendre la fin (environ 20 secondes). Puis Ctrl+F, chercher « ERROR », et noter le nombre trouvé (question 1.3).
5. Taper `feature -LED_STRIP`, Entrée (comme sous Betaflight), puis `save`, Entrée. La carte redémarre : « Connexion ».
6. Onglet Capteurs, case « Accéléromètre », drone posé à plat : « Calibrer ».
7. Batterie chargée (4S pleine : environ 16,8 V au multimètre) ; écrire sa tension dans le cadre du bas. Vol dans un lieu dégagé, personne autour.

![Étape 4 : le fichier diff all de Betaflight collé d'un coup dans le CLI ; aucune ERROR.](photos/TG-P3/avant-4_cli.jpg)

## TG-P3-1. Firmware (onglet Tableau de bord)

- La case Firmware affiche « TRUEGLIDE 2026.6.2 (f9fc6d5) » ?
- Case « Fonctions TrueGlide » : Protection anti-surchauffe, Relance moteur et Sorties commandées (PINIO) sur « Désactivé » ?
- À l'étape 4, Ctrl+F « ERROR » n'a rien trouvé (0 résultat) ?

![TG-P3-1.1 : case Firmware, « TRUEGLIDE 2026.6.2 (f9fc6d5) ».](photos/TG-P3/TG-P3-1.1.jpg)

![TG-P3-1.2 : case Fonctions TrueGlide, tout sur « Désactivé » (le Speeder affiche seulement son angle, 60°).](photos/TG-P3/TG-P3-1.2.jpg)

## TG-P3-2. Baromètre au sol (HÉLICES RETIRÉES)

On vérifie la correction du baromètre avant de voler : le journal Blackbox enregistre la pression et l'altitude.

1. CLI : taper `set debug_mode = BARO`, Entrée, puis `save`, Entrée. « Connexion ».
2. Onglet Blackbox : « Effacer la mémoire interne ». Puis « Déconnecter » et débrancher l'USB.
3. Brancher la batterie. Allumer les lunettes et lancer l'enregistrement vidéo.
4. Drone posé, armer et laisser armé **30 secondes** en regardant l'altitude (en haut à droite de l'OSD). Désarmer.
5. Débrancher la batterie, brancher l'USB, « Connexion ».

- Avant d'armer, `get debug_mode` répond « debug_mode = BARO » ?
- Pendant les 30 secondes, l'altitude de l'OSD reste entre −10 et +10 m (jamais de centaines ni de milliers de mètres) ?
- Onglet Blackbox : « Enregistrer sous… » a créé le fichier du journal (à envoyer) ?
- CLI : `set debug_mode = NONE`, Entrée, `save`, Entrée, « Connexion ». Puis `get debug_mode` répond « debug_mode = NONE » ?

![TG-P3-2.1 : réponse de get debug_mode, « debug_mode = BARO ».](photos/TG-P3/TG-P3-2.1.jpg)

![TG-P3-2.3 : onglet Blackbox, « Effacer la mémoire interne » (étape 2) et « Enregistrer sous… ».](photos/TG-P3/TG-P3-2.3.jpg)

![TG-P3-2.4 : réponse de get debug_mode, « debug_mode = NONE ».](photos/TG-P3/TG-P3-2.4.jpg)

## TG-P3-3. Préparation du vol

1. Onglet Blackbox : « Effacer la mémoire interne ». Puis « Déconnecter » et débrancher l'USB.
2. Monter les 4 hélices, dans le même sens que sous Betaflight.
3. Radio : interrupteur ARM sur arrêt, interrupteur des modes sur ANGLE.

- Les 4 hélices sont montées dans le bon sens et serrées ?
- Batterie branchée, OSD dans les lunettes : la tension affichée est celle d'une batterie pleine (environ 16,8 V) ?

## TG-P3-4. Premier vol en ANGLE (stationnaire, 1 à 2 m de haut, 1 minute)

- Le drone s'arme normalement et décolle droit, sans partir d'un côté ?
- En stationnaire : aucune oscillation, aucune vibration ni bruit moteur anormal ?
- Les commandes répondent comme sous Betaflight (même sens, même douceur) ?
- OSD : l'altitude suit la hauteur réelle (quelques mètres), sans saut ?
- Après l'atterrissage et le désarmement : les 4 moteurs sont tièdes au toucher (on peut garder le doigt posé dessus) ?

## TG-P3-5. Vol en ACRO (vol habituel, 2 à 3 minutes)

- Le drone vole comme sous Betaflight : pas d'oscillation dans les virages, les montées et les descentes ?
- OSD : aucun message d'alerte inattendu (seule l'alerte de batterie basse est normale en fin de vol) ?
- OSD : l'altitude reste cohérente avec le vol, sans saut de centaines ou de milliers de mètres ?
- Après l'atterrissage et le désarmement : les 4 moteurs sont tièdes au toucher ?
- Tension à l'atterrissage au-dessus de 14,0 V (3,5 V par élément) ?

## TG-P3-6. Après les vols (USB branché, batterie débranchée)

- Onglet Blackbox : la mémoire n'est plus vide, et « Enregistrer sous… » a créé le fichier du journal ?
- Onglet « Visualiseur Blackbox » : le journal des vols s'ouvre et s'affiche ?
- Onglet « Ligne de commande (CLI) » : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

![TG-P3-6.1 : onglet Blackbox, mémoire utilisée et « Enregistrer sous… ».](photos/TG-P3/TG-P3-6.1.jpg)

![TG-P3-6.2 : onglet Visualiseur Blackbox, bouton pour ouvrir le fichier du journal.](photos/TG-P3/TG-P3-6.2.jpg)

![TG-P3-6.3 : réponse de diff all dans le CLI.](photos/TG-P3/TG-P3-6.3.jpg)

## À envoyer

### Tension de la batterie avant le vol (multimètre)

### Fichiers envoyés (journal du baromètre, journal des vols, fichier .osd du vol, diff all)

### Réponse de « diff all » (partie 6) [grand]

### Autres remarques (comportement en vol, lignes « ERROR », ce qui a surpris)

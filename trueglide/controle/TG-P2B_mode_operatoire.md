# Essai TG-P2B (TRUEGLIDE 2026.6.2, révision f9fc6d5) : contrôle du baromètre

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), posé à plat sur la table, **HÉLICES RETIRÉES**. Logiciel : TrueGlide
Configurator (https://jschmiedd.github.io/TrueGlide-Configurator/app/), dans Chrome ou Edge sur ordinateur. Matériel :
batterie, radio, lunettes FPV, câble USB. But : vérifier que la révision f9fc6d5 corrige le baromètre (sous la révision
1791465, des mesures fausses faisaient partir l'altitude à des milliers de mètres). Le journal Blackbox enregistre la
pression et l'altitude du baromètre (`debug_mode = BARO`). Les photos ont été prises en déroulant cette fiche dans le
même logiciel, avec le même firmware (révision f9fc6d5, carte simulée) et la configuration KarRmen : ce qu'il faut
regarder est entouré en rouge.

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices.
2. Flasher le fichier trueglide_2026.6.2_STM32F405_SPEEDYBEEF405MINI.hex de la révision f9fc6d5 (envoyé par TrueGlide le 29 octobre) : Betaflight Configurator, onglet « Flasher le Firmware », menu à côté de « Charger un Firmware [Online] » : chargement **local**, choisir ce fichier, cocher « Effacement complet de la puce », « Flasher le Firmware », attendre la fin.
   - **Jamais la mise à jour en ligne** : elle installe la révision 1791465, celle qui a le défaut du baromètre.
   - Ne pas débrancher pendant le flash.
3. Fermer Betaflight Configurator. Ouvrir TrueGlide Configurator, « Connexion ». La fenêtre « Danger » s'ouvre (l'accéléromètre n'est pas encore calibré) : « Fermer ».
4. Onglet « Ligne de commande (CLI) » : ouvrir le fichier `diff all` du test du 28 octobre (« test baromètre BTFL_cli_KARRMEN_20261028_225355 », la même configuration que pour le test Betaflight), copier tout son texte, le coller dans la ligne du bas, Entrée. Le fichier se termine par `save` : la carte redémarre seule. « Connexion ».
5. Onglet « Ligne de commande (CLI) » : taper `set debug_mode = BARO`, Entrée, puis `save`, Entrée. La carte redémarre : « Connexion ».

![Étape 4 : le fichier diff all de test collé d'un coup dans le CLI ; aucune ERROR.](photos/TG-P2B/avant-4_cli.jpg)

## TG-P2B-1. Firmware et réglage (USB branché, batterie débranchée)

- Onglet Tableau de bord : la case Firmware affiche « TRUEGLIDE 2026.6.2 (f9fc6d5) » ?
- Onglet « Ligne de commande (CLI) » : taper `get debug_mode`, Entrée. La réponse est « debug_mode = BARO » ?
- Même onglet : taper `status`, Entrée. La réponse contient « BARO: DPS310 » et « I2C=1 (0 errors) » ?

![TG-P2B-1.1 : case Firmware, « TRUEGLIDE 2026.6.2 (f9fc6d5) ».](photos/TG-P2B/TG-P2B-1.1.jpg)

![TG-P2B-1.2 : réponse de get debug_mode, « debug_mode = BARO ».](photos/TG-P2B/TG-P2B-1.2.jpg)

![TG-P2B-1.3 : réponse de status, lignes BARO: DPS310 et I2C=1 (0 errors).](photos/TG-P2B/TG-P2B-1.3.jpg)

## TG-P2B-2. Contrôle armé (HÉLICES RETIRÉES)

1. Onglet Blackbox : « Effacer la mémoire interne », attendre la fin.
2. « Déconnecter », puis débrancher l'USB. Pas d'USB pendant l'essai.
3. Brancher la batterie. Allumer les lunettes et lancer l'enregistrement vidéo.
4. Drone posé, armer et laisser armé **30 secondes** en regardant l'altitude (en haut à droite de l'OSD). Désarmer.
5. Débrancher la batterie. Brancher l'USB, « Connexion ».

- À l'étape 1, après « Effacer la mémoire interne », la barre de la mémoire est vide et il est écrit seulement « Espace libre 8.0MB » ?
- Pendant les 30 secondes, l'altitude de l'OSD reste entre −5 et +5 m, sans jamais sauter à des centaines ou des milliers de mètres ?
- Onglet Blackbox (étape 5) : « Espace utilisé » apparaît (la mémoire n'est plus vide), et « Enregistrer sous… » a créé le fichier du journal ?

![TG-P2B-2.1 : onglet Blackbox, « Effacer la mémoire interne » ; la barre de la mémoire est vide, « Espace libre 8.0MB ».](photos/TG-P2B/TG-P2B-2.1.jpg)

![TG-P2B-2.3 : onglet Blackbox, « Enregistrer sous… » (grisé sur la photo : la carte simulée n'a pas d'enregistrement ; sur la vraie carte, « Espace utilisé » s'affiche et le bouton est actif).](photos/TG-P2B/TG-P2B-2.3.jpg)

## TG-P2B-3. Remise du réglage

1. Onglet « Ligne de commande (CLI) » : taper `set debug_mode = NONE`, Entrée, puis `save`, Entrée. La carte redémarre : « Connexion ».

- Onglet « Ligne de commande (CLI) » : taper `get debug_mode`, Entrée. La réponse est « debug_mode = NONE » ?
- Même onglet : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

![TG-P2B-3.1 : réponse de get debug_mode, « debug_mode = NONE ».](photos/TG-P2B/TG-P2B-3.1.jpg)

![TG-P2B-3.2 : réponse de diff all dans le CLI.](photos/TG-P2B/TG-P2B-3.2.jpg)

## À envoyer

### Fichier du journal Blackbox (partie 2) : nom du fichier envoyé

### Réponse de « diff all » (partie 3) [grand]

### Autres remarques (lignes « ERROR », ce qui a surpris)

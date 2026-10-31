# Essai TG-P2B (TRUEGLIDE 2026.6.2, révision f9fc6d5) : contrôle du baromètre

Date : …… Drone : …… Fait par : ……

Drone : KarRmen (SpeedyBee F405 Mini), posé à plat sur la table, **HÉLICES RETIRÉES**. Logiciel : TrueGlide
Configurator (https://jschmiedd.github.io/TrueGlide-Configurator/app/), dans Chrome ou Edge sur ordinateur. Matériel :
batterie, radio, lunettes FPV, câble USB. But : vérifier que la révision f9fc6d5 corrige le baromètre (sous la révision
1791465, des mesures fausses faisaient partir l'altitude à des milliers de mètres). Le journal Blackbox enregistre la
pression et l'altitude du baromètre (`debug_mode = BARO`).

## Avant de commencer (rien à cocher)

1. Retirer les 4 hélices.
2. Flasher le fichier trueglide_2026.6.2_SPEEDYBEEF405MINI_253de3b.hex (envoyé par TrueGlide le 31 octobre, il corrige
   le pilote I2C : cause probable des lectures fausses du baromètre) : Betaflight Configurator, onglet « Flasher le
   Firmware », chargement **local**, choisir ce fichier, cocher « Effacement complet de la puce », « Flasher le
   Firmware », attendre la fin. Jamais la mise à jour en ligne (ancienne révision 1791465).
3. Fermer Betaflight Configurator. Ouvrir TrueGlide Configurator, « Connexion ». La fenêtre « Danger » s'ouvre (l'accéléromètre n'est pas encore calibré) : « Fermer ».
4. Onglet « Ligne de commande (CLI) » : coller le dernier `diff all` (fin de TG-P3A, 30 octobre), Entrée. Il se termine
   par `save` : la carte redémarre seule. « Connexion ». Puis taper `feature -LED_STRIP`, Entrée, `save`, Entrée.
5. Onglet « Ligne de commande (CLI) » : taper `set debug_mode = BARO`, Entrée, puis `save`, Entrée. La carte redémarre : « Connexion ».

## TG-P2B-1. Firmware et réglage (USB branché, batterie débranchée)

- Onglet Tableau de bord : la case Firmware affiche « TRUEGLIDE 2026.6.2 (253de3b) » ?
- Onglet « Ligne de commande (CLI) » : taper `get debug_mode`, Entrée. La réponse est « debug_mode = BARO » ?
- Même onglet : taper `status`, Entrée. La réponse contient « BARO: DPS310 » ?

## TG-P2B-2. Contrôle armé (HÉLICES RETIRÉES)

1. Onglet Blackbox : « Effacer la mémoire interne », attendre la fin.
2. « Déconnecter », puis débrancher l'USB. Pas d'USB pendant l'essai.
3. Brancher la batterie. Allumer les lunettes et lancer l'enregistrement vidéo.
4. Drone posé, armer et laisser armé **2 minutes** en regardant l'altitude (en haut à droite de l'OSD). Désarmer.
5. Débrancher la batterie. Brancher l'USB, « Connexion ».

- À l'étape 1, après « Effacer la mémoire interne », la barre de la mémoire est vide (tout l'espace est libre, environ 8 Mo) ?
- Pendant les 2 minutes, l'altitude de l'OSD reste entre −10 et +10 m (une lente dérive de quelques mètres est normale), sans jamais sauter à des centaines ou des milliers de mètres ?
- Onglet « Ligne de commande (CLI) » (étape 5) : taper `status`, Entrée. Noter dans « Autres remarques » le nombre
  d'erreurs I2C (« I2C=1 (n errors) » : depuis cette révision, il compte aussi les erreurs de bus, pas seulement les
  délais dépassés).
- Onglet Blackbox (étape 5) : « Espace utilisé » apparaît (la mémoire n'est plus vide), et « Enregistrer sous… » a créé le fichier du journal ?

## TG-P2B-3. Remise du réglage

1. Onglet « Ligne de commande (CLI) » : taper `set debug_mode = NONE`, Entrée, puis `save`, Entrée. La carte redémarre : « Connexion ».

- Onglet « Ligne de commande (CLI) » : taper `get debug_mode`, Entrée. La réponse est « debug_mode = NONE » ?
- Même onglet : taper `diff all`, Entrée, puis copier tout le texte dans le cadre du bas ?

## À envoyer

### Fichier du journal Blackbox (partie 2) : nom du fichier envoyé

### Réponse de « diff all » (partie 3) [grand]

### Autres remarques (lignes « ERROR », ce qui a surpris)

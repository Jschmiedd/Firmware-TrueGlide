# Les 5 PR Betaflight expliquées ligne par ligne (pour Johann)

Règle de Betaflight (AGENTS.md) : l'auteur doit pouvoir expliquer chaque ligne **sans outil**. Cette fiche sert à ça.
Lis-la une fois par PR, puis réexplique-la avec tes mots. Si une ligne n'est pas claire, demande avant l'envoi.

## PR 1. Nacelle GM3 : un octet faux (firmware)

**Fichiers :** `src/main/io/gimbal_control.c`, 1 ligne, plus 1 fichier de test (`src/test/unit/gimbal_control_unittest.cc`,
2 tests) et son entrée dans `src/test/Makefile`.

**Ce que fait ce code.** La carte reçoit des octets sur le port de la nacelle. Un ordre pour la nacelle commence par
2 octets fixes : `A5` puis `5A`. La carte garde le `A5` en attente pour voir si `5A` suit. Si autre chose suit, ce
n'était pas un ordre : la carte renvoie les octets tels quels.

**Le défaut.** Pour renvoyer le `A5` gardé, le code écrit `GIMBAL_CMD_S && 0xff`.
1. `&&` est un « ET logique » : il répond seulement vrai (1) ou faux (0).
2. `&` est un « ET bit à bit » : il garde l'octet du bas, ici `A5`.
3. Avec `&&`, la carte envoie `01` au lieu de `A5`.

**La correction.** `&&` devient `&`. La ligne au-dessus (`GIMBAL_OP1`) utilise déjà `&`.

**La preuve.**
1. Au simulateur (SITL), on envoie `A5 12` : avant la correction, la nacelle reçoit `01 12` ; après, `A5 12`.
2. Test automatique 1 : `A5 12` doit ressortir tel quel. Sans la correction, il ressort `01 12` (le compilateur
   clang refuse même de compiler l'ancienne ligne : il signale le `&&` avec une constante).
3. Test automatique 2 : `12 34` (aucun en-tête) doit ressortir tel quel. Il passe avant et après : il vérifie que rien
   d'autre n'a changé.

**Quand ça arrive en vrai.** Quand un `A5` passe dans le flux de la nacelle sans être suivi de `5A`.

## PR 2. Courant affiché à 655 A au repos (Betaflight App)

**Fichier :** `src/js/msp/MSPHelper.ts`, 1 ligne, plus 1 test.

**Ce que fait ce code.** Il lit le message `MSP_BATTERY_STATE` envoyé par la carte : la tension, les mAh consommés, et
le courant.

**Le défaut.** La carte envoie le courant **avec un signe** (il peut être un peu négatif au repos, par exemple −0,02 A).
L'App le lit **sans signe** (`readU16`).
1. −2 en 16 bits avec signe, c'est le nombre 65534 sans signe.
2. 65534 / 100 = 655,34 A.

**La correction.** `readU16()` devient `read16()`, c'est-à-dire avec signe. Le message `MSP_ANALOG` lit déjà le même
courant avec `read16()`.

**La preuve.** Le test envoie −0,02 A. Sans la correction, l'App lit 655,34. Avec, elle lit −0,02.

## PR 3. Baromètre faux après une erreur I2C (firmware) : en brouillon

**Fichier :** `src/platform/STM32/bus_i2c_stm32f4xx.c`, 2 lignes.

**Ce que fait ce code.** Pour lire un registre du baromètre, la carte fait 2 étapes :
1. elle écrit le numéro du registre à lire ;
2. elle relit les octets.

Une variable, `subaddress_sent`, retient que l'étape 1 est faite.

**Le défaut.** Si une erreur arrive pendant l'étape 2 (bruit sur le bus), le pilote abandonne la lecture. Mais
`subaddress_sent` **reste à 1**. À la lecture suivante, la carte croit que l'étape 1 est déjà faite. Elle **saute
l'étape 1** et relit les octets à partir du mauvais registre. Le baromètre donne alors des valeurs absurdes, par exemple
−7540 hPa et 258 °C.

**La correction.**
1. Dans `i2c_er_handler()` (quand une erreur arrive) : `memset(&i2c_ev_state[device], 0, …)` remet cette mémoire à
   zéro.
2. Dans `i2cInit()` (au démarrage et à chaque réinitialisation du bus) : la même remise à zéro.

**La preuve.** Tes tests A et B du 13/11 (USB seul, sans batterie, donc moteurs sans courant) :
1. A, sans la correction : la pression plonge toutes les 5 s ;
2. B, avec la même remise à zéro à la fin de l'abandon simulé : la pression reste stable à 961 hPa.

**Pourquoi en brouillon.** Betaflight exige un journal Blackbox d'un **vrai vol** avec le firmware de la PR. Le vol
TG-P3B se fait sur TRUEGLIDE, il ne compte donc pas. Il faudra un vol à part.

## PR 4. La molette fait tourner un moteur sans le vouloir (Betaflight App)

**Fichier :** `src/components/tabs/MotorsTab.vue`, 7 lignes (dont 2 de commentaire), plus 2 tests.

**Le défaut.** Pendant le test moteurs, la molette au-dessus d'une colonne change le moteur de +25 par cran, même si
on n'a jamais cliqué son curseur.

**La correction.** Avant d'agir, le code regarde si le curseur de cette colonne est **sélectionné** (`activeElement`,
l'élément qui a le « focus », après un clic). S'il ne l'est pas, la molette ne fait rien.

**La preuve.** Test 1 : la molette sans curseur sélectionné n'envoie rien (sur master, elle envoie). Test 2 : la
molette sur un curseur sélectionné envoie toujours +25.

## PR 5. Les moteurs continuent quand la page est cachée (Betaflight App)

**Fichier :** `src/composables/motors/useMotorTesting.ts`, 10 lignes (dont 2 de commentaire), plus 3 tests.

**Le défaut.** Si on passe sur un autre onglet pendant le test, les moteurs continuent. La carte garde la dernière
valeur sans limite de temps.

**La correction.**
1. Une petite fonction, `stopWhenHidden`, coupe le test si la page est cachée.
2. Elle écoute l'événement `visibilitychange` du navigateur. Elle est branchée au début du test et débranchée à la
   fin, comme celle du clavier.
3. Couper le test envoie la valeur d'arrêt, comme quand on appuie sur une touche.

**La preuve.** Test 1 : page cachée, le test se coupe et 1 ordre d'arrêt part (sur master, rien). Test 2 : page
visible, le test continue. Test 3 : test arrêté, la page cachée n'envoie rien.

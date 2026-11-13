# TRUEGLIDE : erreurs trouvées dans le code d'origine Betaflight

Document du 13/11/2026. Base comparée : firmware **Betaflight 2026.6.2** (`e0b7bb01b`) et logiciel **Betaflight App
2026.6.2** (`091127c`). Chaque point a été revérifié le 13/11 dans le code d'origine et dans le code TRUEGLIDE en ligne
(firmware `cb4e3f0`, logiciel /app/ `7dc6865`).

Pour chaque point : l'erreur, sa cause, l'explication, la solution choisie, le correctif (commits) et les tests.
« Banc » = essai sur le Mini Pouce de Johann (SpeedyBee F405 Mini), hélices retirées. « Test automatique » = test
qui tourne à chaque contrôle GitHub. « Simulateur » = firmware TRUEGLIDE lancé sur ordinateur (SITL).

**Correction d'une réponse précédente.** Le 13/11, j'ai dit que la cause des mesures fausses du baromètre n'était pas
trouvée. C'était faux : elle a été trouvée le 31/10. C'est une vraie erreur du pilote I2C de Betaflight (point 2).
Autre précision : au point 2, la partie « erreur attribuée au bon capteur » ne corrige pas un défaut visible dans
Betaflight, puisque Betaflight ne regarde pas les erreurs I2C du baromètre. Elle n'est utile que parce que TRUEGLIDE
s'en sert.

## Résumé

| N° | Erreur | Où | Gravité | Corrigé dans TRUEGLIDE | Essayé au banc |
|---|---|---|---|---|---|
| 1 | Nacelle GM3 : `&&` au lieu de `&` | firmware | faible | oui | non (pas de GM3) |
| 2 | Pilote I2C F4 : après une erreur, la lecture suivante saute l'adresse du registre (baromètre faux) | firmware | élevée (altitude fausse) | oui | oui |
| 3 | Onglet Moteurs : arrêt calculé avec le mode 3D de l'écran, pas celui de la carte | logiciel | élevée (moteurs partis) | oui | oui |
| 4 | Intensité lue sans signe : −0,02 A affiché 655,34 A | logiciel | faible | oui | non noté |
| 5 | Mode 3D appliqué sans redémarrage | firmware | moyenne | protégé | oui |
| 6 | Molette de la souris qui fait tourner les moteurs en faisant défiler la page | logiciel | moyenne | oui | non |
| 7 | Test moteurs et fenêtres moteurs actifs quand la page est cachée | logiciel | moyenne | oui | oui (sauf sens DShot) |
| 8 | Valeur d'arrêt inconnue : valeur vide envoyée comme 0 | logiciel | moyenne (rare) | oui | en partie |
| 9 | Branche START du gestionnaire d'erreur I2C | firmware | aucune conséquence | rien à faire | — |

---

## 1. Nacelle caméra Caddx GM3 : mauvais octet renvoyé

- **Erreur.** Quand un premier octet d'en-tête de trame n'est pas suivi du second, le pilote renvoie l'octet `1` au
  lieu de l'octet reçu.
- **Cause.** Faute de frappe dans `src/main/io/gimbal_control.c` (ligne 281) :
  `serialWrite(gimbalSerialPort, GIMBAL_CMD_S && 0xff)`. `&&` est un « ET » logique, qui donne 1 (vrai). Il fallait
  le « ET » bit à bit `&`, qui garde l'octet bas.
- **Explication.** Le pilote relaie à la nacelle les octets qui ne sont pas des commandes. Avec l'erreur, un octet faux
  part vers la nacelle. Ça arrive rarement : il faut un en-tête incomplet.
- **Solution.** Remplacer `&&` par `&`.
- **Correctif.** Firmware TRUEGLIDE depuis le 31/10, présent dans `cb4e3f0`. La ligne est marquée « TrueGlide » dans
  le code.
- **Tests.**
  - Simulateur : `trueglide/sitl_bench/test_gimbal_gm3.py` (trames avec en-tête et CRC corrects, renvoi correct d'un
    en-tête incomplet).
  - Test automatique : aucun.
  - Banc : **pas encore essayé avec une vraie GM3**.

## 2. Pilote I2C (cartes F4) : baromètre faux après une erreur du bus

- **Erreur.** Le baromètre DPS310 donne de temps en temps des mesures absurdes : −7 540 hPa et 258 à 275 °C,
  altitude OSD de −5 704 m à +587 m. Vu au banc le 28 et le 29/10.
- **Cause.** Dans `src/platform/STM32/bus_i2c_stm32f4xx.c`, quand une erreur I2C interrompt une lecture, le
  gestionnaire d'erreur libère le bus mais **ne remet pas à zéro l'état de la lecture**. En particulier, l'indication
  « adresse du registre déjà envoyée » reste à 1. La lecture suivante saute alors l'envoi de l'adresse du registre et
  lit le registre sur lequel le capteur pointait déjà : chez le DPS310, ses registres de configuration.
- **Explication.** Les 6 octets lus sont donc faux ensemble, pression et température en même temps, ce qui donne les
  valeurs absurdes. Betaflight officiel ne les montrait pas sur la même carte : les erreurs I2C y étaient plus rares,
  et le bus était plus chargé sous TRUEGLIDE (bande LED activée, compas sur le même bus). Betaflight ne contrôle pas
  non plus les erreurs I2C du baromètre (`busBusy(..., NULL)`) : une mesure faite pendant une erreur est utilisée telle
  quelle.
- **Solution.**
  1. Remettre à zéro l'état de la lecture après chaque erreur et à chaque initialisation du bus (la vraie correction).
  2. Rejeter le cycle du baromètre quand le bus signale une erreur.
  3. Garder l'erreur **pour chaque capteur** : sinon, une lecture réussie du compas effaçait l'erreur du baromètre.
  4. Filet de sécurité : pression hors 300 à 1 200 hPa ignorée, saut de plus de 20 m d'une mesure à l'autre ignoré
     (5 au plus de suite).
  5. Compter les erreurs I2C dans `status`.
- **Correctif.** `1341bd2`, `a01092b`, `d5244da`, `253de3b`, `24a71eb` (31/10). Filtre : `def66f9`, `f9fc6d5` (29/10).
  Fichiers : `bus_i2c_stm32f4xx.c`, `bus_i2c_busdev.c`, `bus_i2c_errors.h`, `sensors/barometer.c`,
  `sensors/baro_step_filter.c`.
- **Tests.**
  - Test automatique `bus_i2c_errors` (4 cas, dont « une lecture du compas n'efface pas l'erreur du baromètre ») et
    `baro_step_filter` (3 cas).
  - Banc : TG-P2B le 29/10 (filtre : 3 mesures fausses, toutes écartées) ; TG-P2C le 31/10 en `24a71eb` (bande LED
    activée, 31,7 s armé, aucune mesure fausse, 0 erreur I2C) ; TG-V1 le 03/11 (vol intérieur, Blackbox sans défaut).
  - Manque : aucun test automatique ne provoque une erreur I2C pendant une lecture pour vérifier la remise à zéro.

- **Ajout du 13/11, corrigé le même jour après l'audit.** Betaflight connaît un défaut voisin : betaflight#15533 (août
  2026), altitude fausse sur des F405 AIO avec baromètre sur I2C1, causée par un changement du DShot **bitbang**
  (`7f2ce3976`), corrigé chez eux par #15538 pour la future 2026.6.3. **Mais le Mini Pouce n'utilise pas le DShot
  bitbang** : sa configuration de carte le désactive (`DEFAULT_DSHOT_BITBANG DSHOT_BITBANG_OFF`, et la sauvegarde du
  08/11 ne le change pas). Cette explication ne s'applique donc **pas** à la carte de Johann ; elle avait été annoncée
  trop vite. La cause des erreurs I2C sur le Mini Pouce reste **non trouvée**. Autre piste, non prouvée : la même
  série de corrections Betaflight répare une macro DMA (`DMA_CLEAR_FLAG`) qui effaçait les mauvais bits, utilisée par
  le DShot par timer (celui du Mini Pouce) et par le SPI. Les corrections Betaflight sont reprises sur la branche
  d'essai `correction-bf-2026.6.3` (audit : reprise conforme, OUI pour firmware d'essai et banc, NON pour annoncer
  qu'elle corrige le baromètre). Mesures au banc à faire avant toute conclusion. **Rien de publié.**

## 3. Onglet Moteurs : valeur d'arrêt calculée avec le mode 3D de l'écran

- **Erreur.** Batterie branchée, onglet Moteurs : changer « Mode 3D » puis « Sauvegarder » **a fait démarrer les
  moteurs**. Johann l'a constaté sur le Mini Pouce le 07/11 (fiche TG-A1, remarque « moteurs partis au Sauvegarder,
  batterie branchée »).
- **Cause.** Dans `src/components/tabs/MotorsTab.vue` de Betaflight App, la valeur d'arrêt (`zeroThrottleValue`)
  vient de l'interrupteur 3D **affiché à l'écran**, pas de la carte. Dès qu'on touche l'interrupteur, le logiciel
  change sa valeur d'arrêt et l'envoie, avant que la carte ait le même réglage.
- **Explication.** En 3D, l'arrêt est 1500 (le milieu). Sans 3D, c'est 1000. Si le logiciel et la carte ne sont pas
  d'accord, une valeur « arrêt » pour l'un est « marche » pour l'autre : en 3D, 1000 = marche arrière à fond.
- **Solution.**
  - Logiciel : la valeur d'arrêt est **relue sur la carte** (MSP_MOTOR), jamais prise sur l'écran. La relecture est
    faite deux fois et doit être cohérente. Après l'enregistrement, l'arrêt n'est renvoyé que si le test a servi.
  - Firmware : à chaque enregistrement, les moteurs sont remis à l'arrêt (`mixerResetDisarmedMotors()`). Une garde
    coupe les moteurs 1 s après le dernier ordre de test.
- **Correctif.** Logiciel `7a0600b`, `63ea57f`, `ebee781` (07/11), sur /app/ depuis le 07/11. Firmware `81fa7d6`,
  `5bf6418` (garde par port), `8529a5e` (bornes du pilote qui tourne), puis `cb4e3f0`.
- **Tests.**
  - Test automatique `trueglide_onglet_moteurs_monte.test.js` : monte le vrai onglet sur une carte DShot simulée. Le
    défaut est **reproduit sur l'ancien code et absent sur le nouveau**. Aussi `trueglide_arret_moteurs.test.js`.
  - Firmware : `msp_motor_guard` (7 cas), `motor_endpoints` (2 cas) ; simulateur `test_msp_motor_safety.py`.
  - Banc : TG-A1-5 bis le 07/11 (3D oui/non + Sauvegarder, batterie branchée : aucun mouvement) ; TG-A1-5C le 07/11
    (3 oui) ; TG-A1-7C le 08/11 en `cb4e3f0` (moteur 1 seul dans les deux sens, moteurs 2 à 4 immobiles, arrêt OK).
  - Relectures indépendantes : 3 relectures, puis OUI de l'audit le 07/11.

## 4. Intensité affichée : 655,34 A au lieu de −0,02 A

- **Erreur.** Au repos, le capteur de courant donne parfois une valeur légèrement négative (−0,02 A). Le logiciel
  affichait 655,34 A. Vu au banc fin octobre.
- **Cause.** Dans `src/js/msp/MSPHelper.js` (MSP_BATTERY_STATE), le logiciel lit l'intensité en **nombre sans signe**
  (`readU16`), alors que le firmware l'envoie **avec signe** (de −320 à +320 A).
- **Explication.** −2 centièmes, lu sans signe, devient 65 534 centièmes, soit 655,34 A.
- **Solution.** Lire l'intensité avec signe (`read16`). La ligne « Chute de tension » devient « Écart avec la batterie
  pleine », avec « — » si l'écart est négatif.
- **Correctif.** Logiciel `31549b5` (28/10).
- **Tests.**
  - Test automatique : **aucun** (à ajouter).
  - Banc : pas de vérification notée après la correction.

## 5. Mode 3D appliqué sans redémarrage (firmware)

- **Erreur.** Quand on enregistre un changement du mode 3D, la carte en applique une partie tout de suite, sans
  redémarrer.
- **Cause.** Comportement d'origine de Betaflight : le réglage 3D est lu en direct par certains calculs, alors que le
  pilote des moteurs garde les réglages du démarrage jusqu'au redémarrage.
- **Explication.** Entre l'enregistrement et le redémarrage, la valeur d'arrêt calculée peut ne plus correspondre au
  pilote qui tourne. Combiné au point 3, ça peut faire partir les moteurs.
- **Solution.** Le comportement de Betaflight est gardé (pas de redémarrage forcé), mais la carte se protège : après
  chaque enregistrement, les moteurs sont remis à l'arrêt. L'arrêt et les bornes sont toujours calculés avec le pilote
  qui tourne, pas avec le réglage en mémoire. La valeur de test est effacée à l'armement.
- **Correctif.** Firmware `81fa7d6` (remise à l'arrêt), `8529a5e` (`drivers/motor.c` : bornes du pilote qui tourne,
  défaut critique C1 trouvé en 3e relecture), dans `cb4e3f0`.
- **Tests.**
  - Test automatique `motor_endpoints` : « protocole changé en mémoire : l'arrêt reste celui du pilote qui tourne »
    (échoue sans la correction) et « arrêt DShot indépendant du 3D ».
  - Simulateur `test_msp_motor_safety.py` : cas C (enregistrement remet à l'arrêt), D et E (3D activé ou coupé puis
    enregistré : arrêt du pilote qui tourne).
  - Banc : TG-A1-5 bis le 07/11 et TG-A1-7C le 08/11.

## 6. Molette de la souris : moteurs qui montent en faisant défiler la page

- **Erreur.** Pendant le test moteurs, faire défiler la page avec la molette, en passant au-dessus des curseurs,
  change la vitesse des moteurs.
- **Cause.** Dans `MotorsTab.vue` de Betaflight App, `onSliderWheel` réagit à la molette dès que la souris est
  au-dessus d'un curseur, sans vérifier qu'on a choisi ce curseur.
- **Explication.** On croit faire défiler la page, et un moteur accélère de 25 à chaque cran.
- **Solution.** La molette n'agit que sur un curseur **sur lequel on a cliqué** (qui a le focus). Sinon, la page
  défile normalement. La molette est aussi sans effet quand la carte est armée par la radio.
- **Correctif.** Logiciel `ebee781` (molette bloquée si armé), `391767f` (audit M1, curseur choisi), sur /app/ depuis
  le 07/11.
- **Tests.**
  - Test automatique « audit M1 : faire défiler la page au-dessus des curseurs (sans focus) ne change pas les
    moteurs ».
  - Banc : **pas encore**.

## 7. Page cachée : test moteurs et fenêtres moteurs toujours actifs

- **Erreur.** Si on change d'onglet du navigateur ou qu'on réduit la fenêtre pendant que des moteurs tournent, le test
  continue. C'est vrai pour le test moteurs, pour la fenêtre « Réordonner les moteurs » et pour la fenêtre « Sens de
  rotation DShot ».
- **Cause.** Betaflight App ne surveille pas si la page est cachée (aucun `visibilitychange` dans
  `useMotorTesting.js`, `MotorOutputReorderingDialog.vue` ni `EscDshotDirectionDialog.vue`).
- **Explication.** Quand la page est cachée, le navigateur ralentit ses minuteries : les moteurs tournent sans que le
  logiciel les surveille vraiment.
- **Solution.** Page cachée : le test moteurs est coupé (arrêt envoyé), et les deux fenêtres se ferment en arrêtant
  les moteurs.
- **Correctif.** Logiciel `ebee781` (test moteurs), `391767f` (audit M2, fenêtres), sur /app/ depuis le 07/11.
- **Tests.**
  - Tests automatiques : « onglet caché pendant le test : arrêt envoyé », « quitter l'onglet pendant le test : arrêt
    envoyé », `trueglide_dialogues_caches.test.js` (sens DShot, ordre des moteurs, page visible = fenêtre ouverte).
  - Banc : TG-A1-7A le 08/11 (page cachée : arrêt immédiat) et TG-A1-7B le 08/11 (« Réordonner » : fenêtre fermée,
    moteur arrêté).
  - Banc manquant : fenêtre « Sens de rotation DShot ».

## 8. Valeur d'arrêt inconnue : valeur vide envoyée comme 0

- **Erreur.** Si le logiciel n'a pas une valeur d'arrêt sûre, Betaflight App envoie quand même une valeur : celle de
  l'écran, ou une valeur vide qui part comme **0**.
- **Cause.** Le code d'origine part du principe que la lecture de la carte réussit toujours. Une valeur manquante
  (`undefined`) devient 0 quand elle est écrite dans la trame MSP. Le contrôle « 3D entre 1425 et 1575 » ne l'arrête
  pas : une valeur vide n'est ni plus grande ni plus petite.
- **Explication.** Ça peut arriver si une réponse de la carte est perdue (câble, hub USB), si la carte redémarre
  pendant que l'onglet est ouvert, ou si un réglage moteur est changé ailleurs (CLI, radio). En 3D, 0 ou 1000 ne sont
  pas des arrêts.
- **Solution.**
  - Logiciel : valeur d'arrêt inconnue = test refusé et **rien n'est envoyé**. Relecture double et cohérente avec le
    3D de la carte, carte désarmée, aucun tour moteur en télémesure. Les fenêtres moteurs sont refusées tant que
    l'arrêt n'est pas relu.
  - Firmware : la garde coupe les moteurs **1 s** après le dernier ordre de test, avec la valeur d'arrêt de la carte.
    Elle n'est entretenue que par MSP_SET_MOTOR venant du port qui a lancé le test. MSP_SET_MOTOR est refusé quand la
    carte est armée, et tout est remis à zéro au désarmement.
- **Correctif.** Logiciel `ebee781` (07/11). Firmware `5bf6418` (garde par port), puis `cb4e3f0`.
- **Tests.**
  - Tests automatiques : « relecture douteuse » (3 cas : moteurs à 1500 sur une carte sans 3D, tours moteur en
    télémesure, carte armée), « dialogues moteurs refusés tant que l'arrêt n'est pas relu », « sauvegarde ratée :
    aucun repli sur l'écran ». Firmware `msp_motor_guard` (7 cas).
  - Simulateur : `test_msp_motor_safety.py`, cas A bis (le port du test se contente d'interroger la carte : arrêt en
    environ 1 s) et cas B (Walksnail seul : arrêt).
  - Banc : TG-A1-5C-3 le 07/11 (USB débranché : arrêt seul en 1 à 2 s), mais sur le firmware `ee050eb`, pas
    `cb4e3f0`. La lecture ratée elle-même n'est pas provoquée au banc : elle est difficile à créer sur une vraie
    carte.

## 9. Branche START du gestionnaire d'erreur I2C

- **Erreur relevée.** Revue indépendante du 31/10 (point N2) sur `i2c_er_handler` (`bus_i2c_stm32f4xx.c`). Si une
  erreur arrive pendant l'envoi d'un START, le pilote attend dans l'interruption la fin du START, envoie un STOP, puis
  réinitialise complètement le bus.
- **Cause.** Code d'origine Betaflight, fait exprès pour ne pas bloquer le circuit I2C (« start, stop will hang the
  peripheral »).
- **Explication.** L'attente dans l'interruption est très courte, et la réinitialisation remet le bus en état. Depuis
  le point 2, l'état de la lecture est aussi remis à zéro à la réinitialisation.
- **Solution.** Rien à faire : jugé sans conséquence par la revue.
- **Correctif.** Aucun.
- **Tests.** Aucun spécifique. Les essais du baromètre au banc (point 2) n'ont montré aucun blocage du bus.

---

## Ce qui reste à faire

- Point 4 : ajouter un test automatique de lecture de l'intensité négative.
- Points 6, 7 (sens DShot) et 8 (USB débranché en `cb4e3f0`) : fiche courte au banc, avec une vidéo au téléphone
  (moteurs et écran visibles).
- Point 1 : essai avec une vraie nacelle GM3 si Johann en a une.
- Point 2 : test automatique qui provoque une erreur I2C pendant une lecture.
- Signalements à l'équipe Betaflight (points 1, 2, 3, 4, 6, 7, 8) : rédigés seulement sur demande de Johann, et
  envoyés seulement avec son OK.

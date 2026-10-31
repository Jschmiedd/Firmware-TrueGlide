# TrueGlide

Firmware de contrôleur de vol basé sur **Betaflight 2026.6.2**, avec en plus :

| Ajout | Ce que ça fait | Par défaut |
|---|---|---|
| **Protection anti-surchauffe** (`power_guard_*`) | Limite les gaz si un ESC chauffe trop ou si le courant batterie dépasse une limite | Désactivée |
| **Mode SPEEDER** (`speeder_*`, mode 120) | Transition progressive du stationnaire (fuselage vertical) au vol rapide, en mode ANGLE | Inactif sans interrupteur |
| **Batterie basse** (`battery_arm_min_cell_voltage`, `battery_rth_*`) | Décollage refusé sous une tension par élément ; retour au départ (GPS RESCUE) si la tension reste basse en vol. Jamais de coupure en vol | Désactivée |
| **Santé moteurs** (`motor_desync_recovery`, `dshot_telemetry_fallback`) | Relance un moteur désynchronisé ou calé ; vitesse juste pour le filtre RPM quand la télémétrie DShot se perd | Désactivée |
| **Analyse de journaux** (`trueglide/outils/analyse_blackbox.py`) | Rapport en français des défauts d'un fichier blackbox | — |
| **TrueGlide Studio** (`trueglide/studio/index.html`) | Réglage de départ selon le drone (Tiny Whoop 1,2" à 15"), analyse de configuration, ports (UART) et fichier drone préparés sans la carte, missions GPS (modèles TRUENAV) | — |

Sans réglage TrueGlide, le firmware vole **exactement comme Betaflight 2026.6.2** : les ajouts ne font rien tant
qu'on ne les active pas. Le code de vol est celui de Betaflight ; le fichier .hex, lui, n'est pas identique à celui
de la construction en ligne de Betaflight : il contient toutes les options prévues pour la carte (pilotes de
capteurs, protocoles de récepteur et de télémétrie), inactives tant qu'on ne les règle pas.

## TRUEGLIDE et ses deux dérivés

Trois firmwares : **TRUEGLIDE** (le produit de base) et deux dérivés pour la navigation GPS, **TRUENAV** (combiné)
et **TRUENAV_LIGHT**. Dans `make` : `MODELE=TRUEGLIDE` (défaut), `TRUENAV` ou `TRUENAV_LIGHT`.

| Modèle | Pour qui | Contenu | Fichier |
|---|---|---|---|
| **TRUEGLIDE** | course, freestyle, cinéma | Betaflight 2026.6.2 + ajouts TrueGlide | `trueglide_2026.6.2_<MCU>_<CARTE>.hex` |
| **TRUENAV** | drones qui volent aussi en GPS | STANDARD + navigation complète : missions (mode AUTOPILOT), ALTHOLD, POSHOLD, GPS RESCUE, zone de vol (geofence), missions envoyées par MAVLink | `truenav_…` |
| **TRUENAV_LIGHT** | drones de navigation, longue portée | TRUENAV sans les fonctions de course (launch control, transpondeur de course, écran de pré-armement « spec », statistiques des manches) | `truenav_light_…` |

Betaflight ne construit son moteur de missions que pour le simulateur. Les modèles TRUENAV et TRUENAV_LIGHT le mettent sur
les vraies cartes, y compris les cartes à 512 Ko, où Betaflight ne met même pas le GPS. Mémoire mesurée (Ko) :

| Carte | STANDARD | TRUENAV | TRUENAV_LIGHT |
|---|---|---|---|
| MATEKF411 (512 Ko) | 381 | 467 | 464 |
| SPEEDYBEEF7V3 (F722, 512 Ko) | 397 | 492 | 490 |
| SPEEDYBEEF405MINI (1 Mo) | 546 | 572 | 566 |
| MATEKH743 (2 Mo) | — | 612 | 606 |

Le modèle TRUENAV_LIGHT ne gagne que 3 à 7 Ko : les fonctions de course pèsent peu.

**Ne tient pas partout** (compilation de toutes les cartes, 22 octobre 2026, révision 2fbdcce) : la mémoire déborde
sur 15 cartes en TRUENAV et 10 en TRUENAV_LIGHT (dépassement en octets, TRUENAV / TRUENAV_LIGHT) :
- F722 : ALIENFLIGHTNGF7 (20767 / 16541), ALIENFLIGHTNGF7_RX (19799 / 15557), ALIENFLIGHTNGF7_ELRS (9170 / 4960),
  IFLIGHT_BLITZ_F7_PRO (14741 / 10483), JBF7 (13917 / 9691), JBF7_DJI (13929 / 9719), ZEEZF7V3 (14297 / 10039) ;
  en TRUENAV seulement : SPEEDYBEEF7V3 (2821), MATEKF722SE (2397), GEPRCF722_BT_HD_V3 (1353), JBF7_V2 (937) ;
- F411 : BETAFPVF411RX (5801 / 2721) ; en TRUENAV seulement : GEPRCF411SX1280 (364) ;
- H750 : SPRACINGH7EXTREME (8531 / 4799).
Sur ces cartes, utiliser TRUEGLIDE (ou TRUENAV_LIGHT quand il tient). Le tableau ci-dessus date d'une version plus
ancienne : le firmware a grossi depuis.

Construire : `make CONFIG=<CARTE> MODELE=TRUENAV` (ou `TRUENAV_LIGHT`) ; sur GitHub, **Run workflow**, choix « modele ».
Dans les modèles TRUENAV, rien ne change en vol tant qu'aucun mode de navigation n'est placé sur un interrupteur.
Le GPS RESCUE garde le contrôleur habituel de Betaflight.

**Vérifié** : banc de missions de Betaflight en simulateur (`src/test/sitl/sitl_harness.py`), **19 scénarios sur 19
réussis** sur TrueGlide :
- mission, cap, départ à l'envers, virage serré ;
- décollage, atterrissage, orbite, huit ;
- perte radio (arrêt, poursuite, atterrissage) ;
- zone de vol (atterrissage, retour, retour malgré la perte radio) ;
- GPS RESCUE (retour, cap faux, perte du GPS, descente).

Les modèles se construisent sur F411, F722, F405, H743 et AT32F435. Le simulateur fait voler le GPS RESCUE en version
« mission » ; sur les cartes, TRUENAV garde le GPS RESCUE habituel de Betaflight. **Pas encore vérifié en vol réel.**

**Par rapport à INAV et ArduPilot** :
- **Comme eux** : missions GPS (points de passage, attente, orbite, atterrissage, décollage, changement d'altitude),
  maintien d'altitude et de position, retour à la maison, zone de vol, missions envoyées par MAVLink.
- **Mieux pour le vol** : le pilotage de Betaflight (filtre RPM, réglages simplifiés, réactivité), et les
  protections TrueGlide (moteurs, surchauffe).
- **Moins mûr** : le moteur de missions de Betaflight est récent (2026) et expérimental. INAV (toujours développé)
  et ArduPilot ont des années de vols réels.
- **Pas encore là** :
  - ailes volantes et avions en mission ;
  - suivi de terrain ;
  - logique programmable ;
  - fusion de capteurs avancée d'ArduPilot (EKF3) ;
  - (le planificateur, lui, existe : onglet **Plan de vol** de TrueGlide Configurator, ou Mission Planner par
    MAVLink avec le modèle TRUENAV).
- **Premiers essais** : en terrain dégagé, à basse vitesse, prêt à reprendre la main.

## Compatibilité

TrueGlide garde tout Betaflight 2026.6.2 : **629 cartes** (STM32 F405, F411, F722, F745, G474, H7, AT32F435, APM32,
RP2350…), tous les protocoles moteur (PWM, OneShot, MultiShot, DShot150/300/600, ProShot, DShot bidirectionnel) et le
passthrough 4-way pour régler les ESC avec ESC Configurator ou BLHeliSuite. Betaflight Configurator 2025.12 ou plus
récent (API MSP 1.48).

Ce que chaque ajout TrueGlide demande à l'ESC :

| Firmware des ESC | Protocole conseillé | Filtre RPM, ralenti dynamique | Limite de température | Moteur calé / désynchronisé |
|---|---|---|---|---|
| Bluejay (BLHeli_S flashé) | DShot300 ou 600 | oui | oui (télémétrie étendue, `dshot_edt`) | oui (10 ms) |
| AM32 | DShot300 ou 600 | oui | oui (étendue, ou fil série) | oui (10 ms) |
| BLHeli_32 | DShot300 ou 600 | oui | avec le fil de télémétrie série | oui (10 ms) |
| KISS | DShot | non | avec le fil série | avec le fil série (0,3 s) |
| BLHeli_S d'origine | DShot | non (flasher Bluejay) | non | non |
| BLHeli ancien, SimonK | OneShot125 | non | non | non |
| ESC PWM analogique | PWM | non | non | non |

La limite de courant marche avec n'importe quel ESC si la FC a un capteur de courant, ou avec le courant de la
télémétrie série (`current_meter = ESC`).

**DShot300 ou DShot600** : sur les processeurs **F405, F411, G474 et APM32F4**, quand le DShot bidirectionnel est
actif, **Betaflight remplace lui-même DShot600 par DShot300** et limite la boucle PID à 4 kHz : ces processeurs ne
sont pas assez rapides pour envoyer la commande et lire la réponse en DShot600 à 8 kHz. Ce n'est pas une panne.
Sur F7, H7 et AT32F435, DShot600 et 8 kHz conviennent. TrueGlide Studio applique cette règle selon la carte choisie.

## Nouvelles technologies : radio, vidéo, moteurs

TrueGlide garde tout ce que Betaflight 2026.6.2 sait piloter (réglages vérifiés dans le firmware) :

| Domaine | Pris en charge | Réglage |
|---|---|---|
| Radio série | ExpressLRS et Crossfire (CRSF), Ghost (GHST), FrSky SBUS et FPort, Spektrum SRXL2 et DSM, FlySky IBUS, Jeti EX Bus, Graupner SUMD, MAVLink | `serialrx_provider` |
| Radio intégrée à la carte | ExpressLRS SPI (cartes AIO), FrSky D/X, FlySky, Spektrum | `rx_spi_protocol` |
| Vidéo analogique | OSD MAX7456 ; commande du VTX par SmartAudio, Tramp ou MSP | `osd_displayport_device = MAX7456`, `vtx_*` |
| Vidéo numérique | DJI O3 / O4, Walksnail Avatar, HDZero : OSD par MSP DisplayPort | `osd_displayport_device = MSP`, `vcd_video_system = HD` |

Les nouveaux récepteurs et lunettes réutilisent presque toujours ces protocoles standard : une nouvelle génération
ExpressLRS reste du CRSF, un nouveau système HD affiche son OSD en MSP DisplayPort. Ils marchent donc sans changer le
firmware.

**Récupérer les nouveautés de Betaflight** : les ajouts TrueGlide sont petits et séparés (fichiers à eux, quelques
lignes dans 21 fichiers Betaflight). Essai du 11/10/2026 : fusion de la version de développement de Betaflight
(185 changements après 2026.6.2, dont ExpressLRS sur AT32, OSD DisplayPort réglable par carte, télémétrie CRSF et
MSP revues) dans TrueGlide. 6 conflits, tous des ajouts côte à côte (liste des modes, des modes debug), résolus en
gardant les deux. Ensuite, toutes les suites de tests de la version fusionnée passent (93, la version de
développement en ajoute), simulateur et SPEEDYBEEF405MINI compilés. TrueGlide suivra
donc chaque nouvelle version de Betaflight ; la fusion sera faite à sa sortie officielle, pas sur une version de
développement.

**Moteurs plats (« pancake », flux axial, stator sur circuit imprimé)** : pour le firmware, ce sont des moteurs sans
balais triphasés comme les autres, pilotés par les mêmes ESC. Ce qui change, c'est surtout le nombre d'aimants, souvent
plus élevé : la vitesse électrique que l'ESC doit suivre augmente d'autant. Studio la calcule (KV × tension × pôles / 2)
au-delà de 14 pôles. Les « moteurs planaires » industriels (plateau à navettes en lévitation magnétique) déplacent
une pièce sur un plan, sans hélice : ils ne servent pas à la propulsion d'un drone.

**Moteurs** : le nombre de pôles (aimants) est la donnée de base du filtre RPM et du ralenti dynamique. Studio le
demande, et l'analyse des journaux le mesure dans les vibrations du gyro et signale un `motor_poles` faux.

**Remote ID (identification à distance)** : un module Remote ID (Dronetag, BeeID…) se branche sur un port série.
Il diffuse l'identification lui-même ; il peut aussi servir de GPS à la carte (message `MSP2_SENSOR_GPS`, présent
dans Betaflight depuis 4.5, donc dans TrueGlide). Betaflight n'a aucun blocage de l'armement lié au Remote ID.
L'avis EASA n° 06/2026 (18/09/2026) propose d'abaisser le seuil d'enregistrement et de Remote ID de 250 g à
100 g, et (selon la presse spécialisée) un blocage du décollage sans numéro d'exploitant valide pour les drones
neufs. Ce n'est encore qu'une proposition, pas une loi. Si le texte est adopté, un contrôle avant armement
pourra être ajouté à TrueGlide. Il sera désactivé par défaut et suivra la norme publiée.

## Venir de Betaflight 4.4, 4.5 ou 2025.12

- **Configuration** : depuis 4.4, 56 réglages ont disparu ou changé de nom, et **le sens du D a été inversé en
  2025.12** (`d_roll` était le D maximum et `d_min_roll` le D de base ; maintenant `d_roll` est le D de base et
  `d_max_roll` le maximum). Ne pas recoller un ancien `diff all` tel quel : TrueGlide Studio (onglet « Analyser ma
  configuration ») le convertit automatiquement.
- **Journaux de vol** : `analyse_blackbox.py` lit les journaux de 4.4 à 2026.6. Betaflight 4.4 et 4.5 n'enregistrent la
  vitesse des moteurs qu'avec `set debug_mode = DSHOT_RPM_TELEMETRY` ; avec ce réglage, désynchronisations et
  télémétrie figée y sont aussi analysées.
- **Les nouveautés TrueGlide** (protections, mode SPEEDER) demandent de flasher TrueGlide : elles n'existent pas dans
  un firmware Betaflight plus ancien.

## Obtenir le fichier .hex

Sur GitHub, onglet **Actions** > **TrueGlide** : chaque envoi produit l'archive **trueglide-hex** (cartes
courantes). Pour une autre carte : **Run workflow**, indiquer son nom (par exemple `SPEEDYBEEF405V4`).

Ou sur un PC Linux : `make arm_sdk_install`, `make configs`, puis `make CONFIG=NOM_DE_LA_CARTE` ; le fichier est
`obj/trueglide_2026.6.2_<MCU>_<CARTE>.hex`.

## Mise à jour en ligne (depuis TrueGlide Configurator)

Le Configurator peut télécharger lui-même le firmware de la carte : tableau de bord, carte **Firmware**, **Chercher la
mise à jour**, puis **Mettre à jour** ; ou onglet **Flasher le Firmware**, cadre **Mise à jour en ligne**. Le fichier
est vérifié (somme SHA-256) avant d'être chargé. Si la carte est branchée et que la sauvegarde avant flash est
activée (**Sauvegarde activée**), la configuration est sauvegardée avant le flash et peut être restaurée après. Rien n'est demandé au réseau tant que le pilote ne clique pas.

Publier une version (Johann, au moment de « publie ») :

1. Commiter, puis compiler les cartes voulues (`make CONFIG=… [MODELE=TRUENAV|TRUENAV_LIGHT]`). Attention :
   `make clean CONFIG=X` efface aussi les `.hex` des autres modèles de la carte X.
2. `python3 trueglide/outils/publier_firmware.py --sortie <dossier du site>` : écrit `firmware/index.json` et les
   `.hex.gz`. Seuls les `.hex` compilés depuis ce commit sont publiés ; les autres sont listés, à recompiler.
3. Pousser le dossier du site sur GitHub Pages (dépôt TrueGlide-Configurator, branche `gh-pages` : `https://jschmiedd.github.io/TrueGlide-Configurator/firmware/`). Environ 600 Ko par carte et
   par modèle : toutes les cartes en STANDARD et TRUENAV tiennent sous la limite de 1 Go de GitHub Pages ; publier
   sur une branche sans historique (`gh-pages` remplacée à chaque version) pour que le dépôt ne grossisse pas.

## Flasher (première installation)

1. Dans le Configurator, onglet CLI : `diff all`, **enregistrer le texte** (sauvegarde de l'ancienne configuration).
2. Onglet **Flasher le Firmware** : **Charger un firmware TrueGlide (.hex)**, sélectionner le `.hex` TrueGlide (la
   carte est lue dans le nom du fichier), cocher **Effacement complet de la puce**, puis **Flasher le Firmware**.
3. Refaire la configuration de base (ports, récepteur, modes, OSD). Depuis une version 2025.12, ne pas recoller tout
   l'ancien diff : Studio (onglet « Analyser ») dit ce qui a changé.

## Premier vol en sécurité (« ne rien faire brûler »)

1. **Sans hélices**, batterie avec un *smoke stopper* si possible : onglet Moteurs, vérifier ordre et sens de rotation.
2. Activer la protection : par exemple `set power_guard_esc_temp = 100` (ESC avec télémétrie de température) et
   `set power_guard_current = …` (80 % du calibre total des ESC, capteur de courant nécessaire), puis `save`.
3. Alarmes OSD cohérentes : `osd_esc_temp_alarm`, et le mode debug `POWER_GUARD` pour la boîte noire.
4. Hélices : stationnaire de 30 s, atterrir, **toucher les moteurs**. Tièdes : on peut monter le curseur maître.
   Chauds : baisser le maître ou renforcer le filtrage avant d'aller plus loin.

Aucun logiciel ne peut garantir qu'un drone « vole du premier coup » : un moteur mal monté, une hélice à l'envers ou
un ESC mal réglé restent possibles. Ces étapes sont là pour les détecter sans casse.

## Protection anti-surchauffe / surintensité

| Réglage | Rôle | Défaut |
|---|---|---|
| `power_guard_esc_temp` | °C de l'ESC le plus chaud au-delà desquels les gaz sont limités (0 = arrêt) | 0 |
| `power_guard_current` | ampères batterie au-delà desquels les gaz sont limités (0 = arrêt) | 0 |
| `power_guard_min_throttle` | les gaz disponibles ne descendent jamais sous ce pourcentage | 50 |

Seuls les gaz (collectif) sont limités, jamais les corrections PID : le drone reste stable et pilotable. La limite
démarre aux gaz du moment, descend progressivement, et se relâche (25 %/s) quand on repasse sous la limite.

- La limite agit aussi en maintien d'altitude (ALTHOLD, POSHOLD), mais jamais pendant le GPS RESCUE, l'atterrissage
  du failsafe ou la descente de secours d'une mission : ces descentes gardent tous leurs gaz.
- **`power_guard_current` bien au-dessus du courant de stationnaire** : la limite ne remonte que sous 90 % de
  `power_guard_current`. Si le drone tire plus que cette valeur en stationnaire, les gaz restent au plancher
  (`power_guard_min_throttle`) jusqu'au désarmement, sans pleins gaz pour rattraper une descente.
- La température vient de la télémétrie DShot étendue (`dshot_edt = ON`) ou du fil de télémétrie série ; un ESC
  série qui ne répond plus (10 demandes sans réponse) n'est plus compté.

## Mode SPEEDER

Pour les speeders qui décollent fuselage vertical (90°) et volent vite fuselage presque horizontal.

| Réglage | Rôle | Défaut |
|---|---|---|
| `speeder_cruise_angle` | inclinaison vers l'avant en fin de transition (10 à 80°) | 60 |
| `speeder_transition_rate` | vitesse de la transition (°/s) | 30 |
| `speeder_fpv_mix` | roulis / lacet tournés avec la transition (comme `fpv_mix_degrees`, mais progressif) | OFF |

Mise en place : onglet Modes, assigner **SPEEDER** à un interrupteur ; voler en mode **ANGLE**. Interrupteur actif :
le nez descend progressivement jusqu'à l'angle de croisière, le pilote garde la main autour de cet angle.
Interrupteur coupé, failsafe : retour progressif au stationnaire. GPS RESCUE, ALTHOLD et POS HOLD annulent la
transition.

**Prudence** : la logique est vérifiée (tests unitaires, simulateur), pas encore le comportement en vol réel.
Commencer avec `speeder_cruise_angle = 20`, en altitude, puis monter par pas de 10°.

## Batterie basse

Les moteurs ne sont **jamais coupés en vol** pour une batterie basse : le pilote décide, prévenu par l'OSD
(`BAT LOW`, `LAND NOW`) et les bips, comme dans Betaflight. La radio peut aussi alerter à la voix avec la tension
reçue par la télémétrie. En plus, désactivé par défaut :

| Réglage | Rôle | Défaut |
|---|---|---|
| `battery_arm_min_cell_voltage` | sous cette tension par élément (0,01 V, ex. 370 = 3,70 V), au repos, l'armement est refusé (motif `LOW_BATTERY`) | 0 (arrêt) |
| `battery_rth_cell_voltage` | en vol, sous cette tension par élément pendant tout le délai : retour au départ avec le GPS RESCUE | 0 (arrêt) |
| `battery_rth_delay` | durée (s) pendant laquelle la tension doit rester sous le seuil ; une chute brève sur un coup de gaz remet le compte à zéro | 10 |

- Sans batterie mesurée (nombre d'éléments inconnu), rien n'est bloqué ni déclenché.
- Le retour ne part que si le GPS RESCUE peut vraiment tourner à ce moment (GPS RESCUE présent dans le firmware,
  point de départ, GPS, cap) ; sinon l'OSD affiche `BAT RTH N/A` et le pilote garde la main. Pendant le retour :
  `BAT RTH`. Régler `battery_rth_cell_voltage` suffit : le GPS RESCUE est alors considéré comme configuré, même sans
  interrupteur ni failsafe en mode rescue.
- Le pilote reprend la main par un **nouveau** geste au manche de roulis ou de tangage au-delà de 30 % : un manche
  déjà tenu au moment où le retour part ne l'annule pas. L'OSD affiche alors `BAT RTH OFF` pendant 3 s, et le retour
  ne se relance plus avant le prochain désarmement.
- Retour au départ seulement avec le GPS RESCUE : **TRUENAV**, **TRUENAV_LIGHT**, et **TRUEGLIDE** sur les cartes à
  1 Mo et plus.

Vérifié : 7 tests unitaires (seuils, délai, chute brève, reprise en main, GPS indisponible, arrêt) ; réglages acceptés
par le simulateur ; cartes SPEEDYBEEF405MINI (TRUEGLIDE, TRUENAV) et MATEKF411 compilées. Le simulateur n'a pas de
batterie : le déclenchement en vol n'a **pas encore été essayé** (à faire sur la carte, hélices retirées pour le refus
d'armement, puis en vol prudent pour le retour).

## Température des ESC sur la radio (`crsf_tlm_esc_temp`)

Optionnel, **désactivé par défaut**. Activé, la carte envoie toutes les 2 s une trame de température CRSF (type 0x0D,
une valeur par moteur en dixièmes de degré, source 0) : EdgeTX 2.10 ou plus récent l'affiche comme capteurs « Temp » et
permet une alerte vocale. Rien n'est envoyé si un ESC ne donne pas sa température (DShot étendu ou télémétrie série),
ni si les données série sont trop anciennes. Présent seulement si la carte a la télémétrie CRSF et une source de
température ESC. Vérifié en test unitaire ; à essayer en vol avec la radio.

## Quad X pusher (moteurs à l'envers)

Un quad X « pusher » a ses 4 moteurs montés à l'envers sous les bras, avec des hélices pusher. Rien de spécial dans le
firmware : **mixeur Quad X normal** (`mixer QUADX`), mêmes réglages qu'un quad X. Le piège est le sens de rotation :

- le schéma de l'onglet Moteurs et le réglage « Sens de rotation inversé » (`yaw_motors_reversed`) se lisent **vu de
  dessus** du drone ; en regardant un moteur par en dessous, on voit le sens inverse ;
- hélices retirées, vérifier l'ordre et le sens de chaque moteur **vu de dessus** ;
- l'hélice travaille comme sur un quad classique : **face marquée vers le haut** (contre la cloche du moteur inversé)
  et de la **même main** qu'à cette position sur le schéma. Une hélice de l'autre main souffle vers le haut ; une hélice
  de la bonne main face marquée vers le bas souffle vers le bas mais travaille à l'envers (moins de poussée, chauffe).

L'Assistant de réglage (et Studio) a le choix de châssis « Quad X pusher (moteurs à l'envers) » : mêmes calculs qu'un
quad X, avec ces conseils.

## Nacelle caméra Caddx GM3

Le pilote de la nacelle **Caddx GM3** (port série, 115 200 bauds) est activé dans les firmwares TrueGlide, sauf sur
les cartes F411 et dans le modèle TRUENAV_LIGHT (place en mémoire : environ 2,2 Ko). Il ne fait rien tant qu'aucun port
série n'a la fonction « Gimbal » (TrueGlide Configurator : onglet Ports, « Nacelle Caddx GM3 » ; CLI :
`serial <port> 262144 115200 57600 0 115200`). La nacelle suit les manches et le suivi de tête des lunettes ; réglages
`gimbal_*` (gains, décalages, limites, `gimbal_stabilisation` 0 à 2, `gimbal_sensitivity`).

Correction TrueGlide dans le pilote d'origine : quand un premier octet d'en-tête n'était pas suivi du second, le pilote
renvoyait `1` au lieu de cet octet (`&&` à la place de `&`).

Vérifié sur le simulateur (`trueglide/sitl_bench/test_gimbal_gm3.py`) : trames à la nacelle avec en-tête et CRC
corrects, renvoi correct d'un en-tête incomplet. **Pas encore essayé avec une vraie GM3.** Dans le simulateur seulement,
un port TCP sans client au bout finit par bloquer la boucle (le tampon ne se vide pas) : brancher un client sur le port.

## Hexa et X8 coaxial

Les mixeurs HEX6X et OCTOX8 de base fonctionnent tels quels. TrueGlide suit tous les moteurs : santé moteurs et
relance sur 8 moteurs, mode debug `MOTOR_HEALTH` (relances des moteurs 1 à 8), alertes OSD `M1` à `M8 RESTART`,
outils de journaux jusqu'à 8 moteurs. L'Assistant de réglage (et Studio) a un choix de châssis : Quad X, Hexa X,
X8 coaxial, Quad X pusher (moteurs à l'envers) ; pour le X8, la poussée est estimée avec 20 % de perte par moteur (moteurs du bas dans
le flux de ceux du haut), une estimation à vérifier au stationnaire.

## Santé moteurs (DShot bidirectionnel)

La relance est désactivée en mode 3D (moteurs réversibles) : elle n'est prévue que pour les moteurs à un seul sens.

| Réglage | Rôle | Défaut |
|---|---|---|
| `motor_desync_recovery` | relance d'un moteur désynchronisé ou calé | OFF |
| `motor_desync_threshold` | seuil : % de la vitesse attendue en dessous duquel le moteur est jugé décroché (10 à 80) | 40 |
| `dshot_telemetry_fallback` | vitesse d'un moteur dont la télémétrie est perdue : suit les autres au lieu de rester figée | OFF |

Détection : moteur commandé à au moins 30 %, plus lent que `motor_desync_threshold` % de la vitesse attendue (autres
moteurs, au prorata des commandes) **et** que la moitié du plus lent des autres, pendant 10 ms. Il est alors coupé au
ralenti 50 ms, puis remis en 100 ms : l'ESC relance sa commutation. Un moteur réellement bloqué est relancé environ
6 fois par seconde au lieu de chauffer à pleine puissance. Mode debug `MOTOR_HEALTH` : nombre de relances par moteur
(moteurs 1 à 8).

Vitesse minimale : la comparaison n'est faite que si la vitesse attendue dépasse 1 500 tr/min, assez bas pour les
grandes hélices (10 à 15 pouces, 2 500 à 4 500 tr/min en stationnaire). Passage de 5 000 à 1 500 tr/min
vérifié sur tous les journaux disponibles (116 vols de 5 pouces) : relances identiques avant et après (34, sur
les mêmes vols). Les grandes hélices ne sont vérifiées qu'en tests unitaires : pas encore de journal de vol réel. Avec la télémétrie série (BLHeli_32, KISS,
AM32), la vitesse est convertie en tr/min comme en DShot (`motor_poles` doit être juste).

`dshot_telemetry_fallback` ne sert qu'au filtre RPM et au ralenti dynamique : l'OSD et la boîte noire continuent
d'afficher la valeur reçue de l'ESC, figée si la télémétrie est perdue.

Mise au point sur de vrais vols (journaux d'un quad de test, non publiés) : la logique compilée du firmware a été
rejouée sur 26 vols (`trueglide/outils/rejouer_desync.py`) ; elle intervient sur chaque calage, jamais sur les vols
sains. Elle ne répare pas une voie d'ESC défaillante : elle limite les dégâts.

Limites du rejeu : il utilise la boîte noire (1 à 2 kHz, pas la boucle PID complète), juge la télémétrie « fraîche »
quand la valeur change, et ne simule pas l'effet de la relance sur la suite du vol (le moteur continue comme dans le
journal). Il montre où la logique intervient, pas ce que le drone aurait fait ensuite.

### Alertes OSD

Deux alertes TrueGlide s'ajoutent à celles de Betaflight (onglet OSD du Configurator, liste des alertes, élément
« Avertissements » affiché) :

| Alerte OSD | Affiché | Quand |
|---|---|---|
| Gaz limités | `ESC HOT 65%` ou `AMP LIM 80%` | la protection limite les gaz (ESC trop chaud ou courant trop fort) ; le % est la limite réelle des gaz |
| Moteur relancé | `M2 RESTART` (clignote) | pendant 3 s après la relance d'un moteur calé ou désynchronisé |

Elles n'apparaissent que si la protection correspondante est activée. Sur une carte qui garde une ancienne
configuration, cocher les deux alertes dans l'onglet OSD (elles sont cochées d'office après une remise à zéro).

DShot300 ou DShot600 : avec le DShot bidirectionnel, DShot300 suffit pour une boucle PID à 4 kHz et rend la
télémétrie moins sensible aux parasites ; DShot600 est utile pour une boucle à 8 kHz.

## Analyser ses journaux de vol

```
pip install orangebox numpy
python3 trueglide/outils/analyse_blackbox.py mon_vol.bbl -o rapport.md
```

Le rapport **mesure aussi le nombre de pôles (aimants) des moteurs** dans les vibrations du gyro et le compare à
`motor_poles` : c'est la donnée de base du filtre RPM et du ralenti dynamique (fiche « 12N14P » = 14 aimants). Il
ne conclut que si la mesure est nette (gyro brut et vitesse des moteurs enregistrés, quelques secondes à gaz
stables). Mesure de 8 à 48 pôles ; au-delà de 28 environ, l'écart avec le voisin (±2) devient petit et le rapport
l'écrit « environ N (±2) ». Vérifié sur de vrais vols (14 pôles mesurés) et sur des moteurs simulés de 8 à 48 pôles.

Le rapport liste, vol par vol : télémétrie DShot figée par moteur, désynchronisations et calages (logique du
firmware), saturation, bruit gyro, oscillations lentes, avec un conseil sur le protocole DShot et une synthèse par
moteur. Les outils lisent tous les moteurs du journal (hexacoptère, octocoptère : jusqu'à 8).

### Mesures de vol pour le calculateur (`--retour`)

```
python3 trueglide/outils/analyse_blackbox.py mon_vol.bbl [autres.bbl] --retour KarRmen_retour_trueglide.json [--nom KarRmen]
```

Écrit un fichier au format `talktrue-trueglide-retour` (version 2 = version 1 + bloc `centrage`), lu par le calculateur TALKTRUE FPV XRIG. Ce format
est séparé de celui de XPro. Il ne contient que des mesures faites en vol, jamais d'estimation. Une valeur non
mesurée, ou hors limites, vaut `null`.

| Mesure (`mesures.…`) | Unité, type | Limites | Comment |
|---|---|---|---|
| `poles` | nombre d'aimants, entier | 4 à 64 | vibrations du gyro (voir plus haut) |
| `poles_exact` | vrai / faux | — | faux : « environ, ±2 » |
| `tr_min_stationnaire` | tr/min mécaniques, nombre | 500 à 100 000 | télémétrie DShot ; seulement si les pôles sont mesurés |
| `gaz_stationnaire_pct` | %, nombre | 0 à 100 | commande des gaz |
| `tension_stationnaire_v` | V batterie, nombre | 1 à 70 | |
| `courant_stationnaire_a` | A batterie entière, nombre | 0,1 à 500 | capteur de la carte ; étalonnage non vérifié (offset et échelle dans `reglages`) |
| `duree_stationnaire_s` | s, nombre | 3 et plus | temps de stationnaire retenu |
| `saturation_pct` | %, nombre | 0 à 100 | moyenne des vols |
| `desynchronisations` | nombre, entier | 0 et plus | logique du firmware rejouée |
| `telemetrie_figee_pct_max` | %, nombre | 0 à 100 | pire moteur, pire vol |

Stationnaire : fenêtres d'une seconde, drone armé, gaz stables (écart type sous 1,5 %), manches roulis, tangage et
lacet au centre, drone stable, moteurs non saturés. Valeur retenue : médiane des fenêtres, s'il y a au moins 3 s.
`source` donne le firmware, la carte, le nom, la date et les journaux lus. `reglages` donne pour information
`motor_poles`, `motor_kv`, le protocole moteur, `dshot_bidir` et l'offset et l'échelle du capteur de courant.

**Centrage (version 2, protocole `centrage_v1` commun avec le calculateur)** : un bloc par vol lu dans `centrage`.
Vol de mesure : mode Angle, dehors sans vent, 1,5 à 3 m de haut, hélices neuves du même lot, `acc_trim_roll` et
`acc_trim_pitch` à 0, batterie au-dessus de 3,7 V par élément ; au moins 10 s de stationnaire calme, demi-tour en lacet,
encore au moins 10 s (20 s valides au total, 30 conseillées). Fenêtres d'une seconde retenues : armé (sans les 2 s
du début et de la fin), manches roulis / tangage à moins de 2 % en moyenne et jamais au-delà de 10 %, gaz stables,
lacet sous 20 °/s, aucun moteur saturé, régime de chaque moteur connu. Le cap vient du gyro de lacet ; cap A = celui
de la première fenêtre, cap B = A + 180 ± 30°.

| Champ (`centrage[i].…`) | Contenu |
|---|---|
| `protocole` | `"centrage_v1"` si le vol remplit le protocole, sinon `null` |
| `disposition`, `ordre_moteurs` | `quadx` ; 1 arrière droit, 2 avant droit, 3 arrière gauche, 4 avant gauche (Quad X Betaflight) |
| `tr_min` | tr/min mécaniques par position, moyenne des deux caps (retire le vent, pas un moteur plus faible) |
| `caps.A` / `caps.B` | `duree_s`, `tr_min` et `tr_min_ecart_type` par position, `sortie_pct` par position, `inclinaison_deg` (accéléromètre, sans signe) et son écart type |
| `tension_debut_v`, `tension_fin_v`, `cellules` | batterie pendant les fenêtres retenues |
| `refus` | raisons : moins de 20 s, un seul cap ou moins de 5 s d'un côté, inclinaison moyenne au-delà de 3°, régimes dispersés de plus de 5 % (vent), disposition autre que Quad X |

Pour régler les PID à partir de mesures, Betaflight 2026.6 a aussi un onglet **Réglage automatique** (Autotune, excitation « chirp ») :
il reste disponible dans TrueGlide Configurator, à côté de l'Assistant de réglage.

## Avec les outils Betaflight d'origine

Betaflight Configurator, Blackbox Explorer et les scripts de la radio ne connaissent pas les ajouts TrueGlide. Le
vol n'en dépend pas, mais certains noms y sont faux ou absents ; **TrueGlide Configurator** les affiche juste.

| Ajout TrueGlide | Dans les outils Betaflight |
|---|---|
| Motif de refus d'armement `LOW_BATTERY` (`battery_arm_min_cell_voltage`) | affiché **`ARM_SWITCH`** ; un vrai `ARM_SWITCH` apparaît alors sans nom. La commande CLI `status` donne le vrai motif |
| Mode **SPEEDER** (onglet Modes) | lu sur la carte : nom juste, sans texte d'aide |
| Modes debug `POWER_GUARD`, `MOTOR_HEALTH` | sans nom, ou avec un autre nom dans Blackbox Explorer |
| Alertes OSD `ESC HOT` / `AMP LIM`, `M RESTART` | sans nom dans la liste des alertes de l'onglet OSD |

`LOW_BATTERY` est placé juste avant `ARM_SWITCH`, qui doit rester le dernier motif : un autre emplacement donnerait
un nom faux à un motif de Betaflight à la place.

## Vérifications faites

- Tests unitaires Betaflight + TrueGlide : 71 suites, en mode strict (`make EXTRA_FLAGS=-Werror test-all`).
- Construction pour 42 cartes courantes (F405, F411, F722, F745, H743, AT32F435) et pour le simulateur (SITL).
- Simulateur : réglages acceptés et sauvegardés, mode SPEEDER annoncé au Configurator, transition suivie
  (`trueglide/sitl_bench`).
- Studio : 43 tests (dont les 7 types d'ESC sur les 629 cartes) ; 40 drones et une mission collés dans le firmware, toutes les commandes acceptées et PID
  identiques (`trueglide/studio/tools/valider_sitl.py`).

## Licence

TrueGlide est un dérivé de Betaflight 2026.6.2, modifié par TalkTrueFPV depuis octobre 2026, sous **GPL v3**
(fichier `LICENSE`), fourni **sans aucune garantie**. Betaflight et les autres marques citées appartiennent à leurs
titulaires ; TrueGlide n'est ni affilié au projet Betaflight ni approuvé par lui. Pour un usage personnel, le dépôt
peut rester privé. Si le firmware est distribué (fichier .hex donné ou vendu, carte vendue avec), son code source
complet doit être fourni sous la même licence. Ne jamais y mélanger de code privé.

Le logo Talk True FPV (`trueglide/studio/logo_talktruefpv_*.png`) est la marque de Johann (TalkTrueFPV). Il n'est
pas sous GPL : il peut être affiché avec TrueGlide, pas réutilisé pour un autre produit sans son accord.

Sécurité : vérifier chaque réglage, essayer d'abord hélices retirées, puis en vol dans un lieu dégagé. Le pilote reste
seul responsable de son drone et du respect de la réglementation de son pays (en Suisse : OFAC ; en France : DGAC).

Contact : TalkTrueFPV, talktruefpv@gmail.com.

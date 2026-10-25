# TrueGlide Studio

Logiciel de réglage pour le firmware TrueGlide (base Betaflight 2026.6.2). Une seule page : ouvrir
`index.html` d'un double-clic (Chrome, Edge ou Firefox), rien à installer. La carte de l'onglet Navigation a besoin
d'Internet ; sans connexion, le tableau des points fonctionne seul.

## Ce qu'il fait

- **Carte et ESC** : choisir la carte parmi les 629 de Betaflight (processeur, gyroscope, sorties moteur, capteur de
  courant) et le firmware des ESC (Bluejay, AM32, BLHeli_32, KISS, BLHeli_S d'origine, BLHeli ancien, PWM). Studio en
  déduit le protocole moteur, la fréquence de boucle et les protections TrueGlide possibles.
- **Réglage du drone** : à partir de la taille d'hélice (1,2 à 15 pouces : du Tiny Whoop au 15"), des moteurs, de la batterie, de la masse et du
  style (freestyle, cinématique, longue portée, course, speeder), il propose un réglage de départ et les commandes CLI
  à coller dans le Configurator. Le réglage passe par les curseurs du réglage simplifié de Betaflight : on peut
  ensuite l'ajuster à la souris dans l'onglet PID.
- **Venir d'une ancienne version** : un `diff all` de Betaflight 4.4, 4.5 ou 2025.12 est converti pour TrueGlide
  (réglages renommés traduits, sens du D d'avant 2025.12 remis dans le bon ordre, réglages supprimés retirés avec
  leur raison, valeurs vérifiées contre les bornes du firmware). Coller le résultat après le flash, à la place de
  l'ancien diff.
- **Analyser ma configuration** : coller un `diff all` ; il signale les problèmes (DShot bidirectionnel, failsafe,
  protection TrueGlide…) et donne seulement les commandes qui changent.
- **Ports (UART)** : sans la carte, choisir ce qui est branché sur chaque port (récepteur ExpressLRS/Crossfire, Ghost,
  SBUS, IBUS ; GPS ; vidéo numérique DJI/Walksnail/HDZero ; VTX SmartAudio ou Tramp ; télémétrie série des ESC ; MSP ;
  nacelle Caddx GM3). Studio connaît les ports de chacune des 629 cartes et leurs fonctions d'usine (lus dans le
  firmware), part de la configuration collée s'il y en a une, et ne produit que les commandes qui changent
  (`serial`, `feature`, `set`). Il signale un port sans la broche nécessaire, deux récepteurs, et le GPS absent du
  firmware **TRUEGLIDE** des cartes à petite mémoire (F411, F722, G474, H750 : prendre **TRUENAV**).
- **Lecture des broches (sécurité)** : Studio connaît les broches d'usine de chaque carte (ports série, moteurs, LED,
  buzzer, PPM, PINIO, I2C, mesures de tension et de courant…). Si la configuration collée contient des lignes
  `resource` (un `dump all` les donne toutes, un `diff all` seulement celles changées), il en tient compte : port
  dont une broche est passée à `NONE`, port ajouté sur d'autres broches, et surtout **broche d'un port utilisé
  donnée aussi à autre chose** (par exemple le TX du récepteur réaffecté à un moteur : drone non pilotable). Un
  partage qui existe déjà en usine (PPM et RX d'un port sur certaines cartes) n'est pas signalé. Sans ligne
  `resource`, rien ne change. Studio n'écrit jamais de commande `resource` : il dit quoi corriger.
- **Fichier drone** : un fichier `.json` garde le formulaire, les ports et la configuration de la carte (`diff all`).
  On l'enregistre, on le rouvre plus tard sans la carte ; un `diff all` enregistré depuis le Configurator s'ouvre
  aussi (carte et nombre de pôles repris). Les commandes se collent ensuite dans l'onglet CLI, drone branché.
- **Fichier du calculateur TALKTRUE FPV XRIG** : le calculateur exporte un `.json` au format propre à TrueGlide
  `talktrue-trueglide-studio` (séparé du format de XPro, qui est refusé). Studio l'ouvre comme un fichier drone et
  remplit le formulaire (voir « Format du calculateur » plus bas).
- **Navigation GPS** : mission par points (mode AUTOPILOT de Betaflight 2026.6), retour GPS, barrière virtuelle.
  Il produit les commandes `waypoint` exactes du firmware (altitudes converties au-dessus du niveau de la mer).

## D'où viennent les chiffres

Les PID de départ ne sont pas inventés : ce sont les moyennes des préréglages publiés par des régleurs reconnus dans le
dépôt officiel `betaflight/firmware-presets` (firmware 2025.12) :

| Taille | P | I | D | FF | Préréglages |
|---|---|---|---|---|---|
| 1,2" (Tiny Whoop 65 mm) | 73,5 | 131,0 | 53,0 | 173,5 | UAV Tech Whoop (1S & 2S), AOS 65mm (masse de référence 25 g : estimation) |
| 3,5" | 63,8 | 109,9 | 44,6 | 173,1 | SupaflyFPV 3/4", UAV Tech Micro, AOS 3.5, AOS 4 |
| 5" | 73,1 | 122,9 | 47,5 | 218,3 | UAV Tech 5" 650-750 g, AOS 5 |
| 7" | 54,0 | 85,0 | 49,6 | 169,9 | AOS 7, AOS UL7, UAV Tech 6-7", SupaflyFPV 7" |
| 10" | 35,5 | 63,5 | 35,4 | 174,0 | AOS UL10, UAV Tech 10" |

- Entre deux tailles : interpolation. **Au-delà de 10" (jusqu'à 15")** : prolongement de la tendance 7" → 10", 15 %
  plus bas pour la prudence, et signalé « extrapolé » : il n'existe pas de préréglage public de référence pour ces
  tailles. Monter ensuite le curseur maître par pas de 5.
- Correction selon la puissance (± 20 % au plus), entre 1,2" et 10" seulement : la poussée est estimée par la même
  formule pour ton drone et pour la référence de même taille, seule la comparaison compte.
- Tiny Whoop (sous 2") : `pidsum_limit` 1000, compensation de la chute de tension, ralenti dynamique doux
  (`dyn_idle_p_gain` 40, AOS 65mm), rappel des moteurs à 12 aimants, filtre RPM présenté comme indispensable.
- Filtres : options « qualité de construction » des préréglages UAV Tech. Ralenti dynamique : préréglages de la même
  taille. Filtre RPM des grandes hélices : sous la fréquence du moteur au ralenti.

## Vérifications

- `node --test trueglide/studio/test/studio.test.js` : 45 tests (bornes de tous les réglages pour 2 à 15", 5 styles, 3 châssis ;
  format des commandes de mission ; analyse d'un diff ; broches lues, dont un `dump all` des broches d'usine des 629
  cartes sans aucune fausse alerte).
- `python3 trueglide/studio/tools/valider_sitl.py` (après `make TARGET=SITL`) : colle les commandes générées pour 40
  drones et une mission dans le firmware compilé pour PC. Résultat du 11 octobre 2026 : toutes acceptées, PID calculés
  par le firmware identiques au chiffre près à ceux affichés par Studio, mission relue à l'identique. (Le SITL n'a pas
  de DShot : `dshot_bidir`, `dyn_idle_min_rpm` et `rpm_filter_min_hz` y sont vérifiés seulement contre les bornes du
  code source.)
- `python3 trueglide/studio/tools/valider_sitl.py` vérifie aussi les ports : chaque composant placé sur un port du SITL,
  commandes acceptées et fonctions relues par la commande `serial` (le SITL n'a ni récepteur série, ni OSD, ni VTX :
  ces réglages y sont signalés absents ; ils existent dans le firmware des cartes).
- `tools/extract_cartes.py` régénère `js/cartes.js` (les 629 cartes, avec leurs ports série) après `make configs`.
- `tools/extract_settings.py` régénère `js/settings_db.js` (les 831 réglages du firmware, y compris les textes et tableaux, bornes et choix) après une
  mise à jour du firmware.

## Format du calculateur (`talktrue-trueglide-studio`, version 1)

Exemple complet : `exemples/calculateur_exemple.json`. Une donnée inconnue vaut `null` (jamais une valeur supposée) :
Studio laisse alors son champ tel quel et le signale. Une valeur hors limites n'est pas reprise et est signalée.

| Champ (`drone.…`) | Unité, valeurs | Champ de Studio | Obligatoire |
|---|---|---|---|
| `helice_pouces` | pouces, 1,2 à 15 | Taille d'hélice | oui |
| `kv` | tr/min/V, 100 à 30 000 | KV des moteurs | oui |
| `cellules` | nombre de S, 1 à 14 | Batterie | oui |
| `masse_g` | grammes, batterie comprise, 15 à 20 000 | Masse en vol | oui |
| `poles` | nombre d'aimants, pair, 4 à 64 | Pôles des moteurs | oui |
| `disposition` | `quadx`, `hexa`, `x8`, `quadx_pusher` | Type de châssis | oui |
| `esc_firmware` | `bluejay`, `blheli_s`, `am32`, `blheli_32`, `kiss`, `blheli`, `pwm` | Firmware des ESC | oui |
| `esc_calibre_a_par_voie` | ampères en continu, par voie | Calibre ESC | non |
| `esc_pwm_khz` | 24 ou 48 | Fréquence PWM des ESC | non |
| `carte` | nom exact de la carte Betaflight | Carte de vol | non |
| `capteur_courant` | vrai / faux | Capteur de courant sur la FC | non |
| `telemetrie_serie_esc` | vrai / faux | Fil de télémétrie série branché | non |

Autres champs : `format`, `version` (1), `source`, `date`, `nom` ; `valeurs_supposees` (liste des champs supposés par le
calculateur, affichés « à vérifier ») ; `etude.poussee_max_g_par_moteur` et `etude.courant_stationnaire_a`, affichés
repris dans « Poussée max par moteur, calculée » et « Courant au stationnaire, calculé ».
Définitions du calculateur : poussée statique d'un moteur seul avec son hélice, à pleins gaz, en grammes, tirée du banc
d'essai du fabricant et ramenée à l'hélice, au nombre de cellules (tension nominale) et à la densité de l'air ; sans
perte coaxiale (Studio l'applique en X8). Courant au stationnaire : courant total de la batterie, estimé par le modèle du
calculateur (pas une mesure).

**Base du réglage** (formulaire Mon drone) : « Estimations de Studio » (par défaut) ou « Données du calculateur ».
Avec les données du calculateur :
- la correction des PID reste sur l'estimation : elle compare le drone à des drones de référence dont on n'a que
  l'estimation ; une poussée calculée d'un seul côté fausserait la comparaison (les PID sont donc les mêmes) ;
- Studio affiche le rapport poussée / poids calculé (perte coaxiale de 20 % comprise en X8) et prévient sous 2 ;
- il propose `ap_hover_throttle` (gaz de stationnaire des maintiens et du retour GPS), estimé à partir de la poussée
  calculée (poussée proportionnelle au carré de la commande, ralenti moteur 5,5 %) : à vérifier dans le journal du
  premier vol ;
- il prévient si le courant au stationnaire par moteur dépasse la moitié du calibre des ESC.
Pour que la correction des PID utilise aussi le calcul, il faudrait que le calculateur calcule les mêmes valeurs pour
les drones de référence (tableau « D'où viennent les chiffres »).

## Limites

Un réglage de départ n'est pas un réglage fini : chaque drone doit être essayé prudemment (stationnaire court,
température des moteurs), puis ajusté. Les drones de plus de 10" et le mode speeder demandent encore plus de prudence.

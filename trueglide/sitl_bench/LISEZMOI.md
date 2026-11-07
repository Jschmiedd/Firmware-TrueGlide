# Banc d'essai SITL (TrueGlide)

Petit simulateur physique de quadri X (Python, sans dépendance) branché sur le firmware compilé pour PC
(`make TARGET=SITL`), via le protocole du pont Gazebo de Betaflight (UDP 9002 à 9004) et MSP / CLI (TCP 5761).

- `test_axes.py` : vérifie que les axes du simulateur et ceux du firmware concordent (gyro, accéléromètre).
- `test_speeder_court.py` : stationnaire puis interrupteur SPEEDER (croisière 30°, 60°/s) et retour ;
  affiche l'inclinaison visée par le firmware et celle du drone simulé.

Lancer depuis ce dossier, après `make TARGET=SITL` : `python3 test_speeder_court.py`.

## Limite connue (à lire)

Le simulateur est cadencé sur le temps réel en Python et l'attitude estimée par le firmware s'écarte de
l'attitude « vraie » du simulateur dès que le drone accélère à l'horizontale. Il sert à vérifier la
**logique** du firmware (sens des commandes, programme de transition, modes, réglages), pas à juger la
qualité du vol : ça, seul un vol réel prudent le dit.

Résultat du 11 octobre 2026 (`test_speeder_court.py`) : l'inclinaison visée par le firmware suit le programme
attendu (0 → 30° en 0,5 s, maintien, retour vers 0), nez vers le bas comme le manche avant.

## Sécurité du test moteurs par MSP (07/11/2026, TG-A1-5)

`test_msp_motor_safety.py` (sans le simulateur physique) : lance le SITL dans un dossier temporaire, met l'UART2 en MSP
(second appareil MSP, comme une vidéo HD Walksnail), puis vérifie avec MSP_MOTOR les valeurs réellement appliquées :
test entretenu par le port du logiciel, arrêt après ~1 s quand seul l'autre port parle, arrêt à l'enregistrement,
arrêt = neutre 3D ou minimum selon le mode enregistré. Code de sortie 0 = 7/7. Limite : le SITL n'a pas de sortie DShot
(0 moteur en DSHOT600) ; il tourne en PWM. Les valeurs DShot (1000 / 1500) sont vérifiées par le test
`test/js/trueglide_onglet_moteurs_monte.test.js` du logiciel (modèle de dshot.c).

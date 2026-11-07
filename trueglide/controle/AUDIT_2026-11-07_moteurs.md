# Dossier pour l'audit indépendant — sécurité du test moteurs (TG-A1-5), 07/11/2026

## Le défaut

Onglet Moteurs, batterie branchée : changer « Mode 3D » puis « Sauvegarder » a fait démarrer les moteurs (essai TG-A1-5,
matin du 07/11). Cause : le logiciel calculait la valeur d'arrêt avec le mode 3D affiché à l'écran, pas celui de la carte.

## Corrections (à relire)

Logiciel `Jschmiedd/TrueGlide-Configurator`, branche `essai-onglets`, de `fe273de` à `ebee781` (publié sur /test/ seulement) :
- 7a0600b, 63ea57f : arrêt relu sur la carte (MSP_MOTOR).
- ebee781 : arrêt inconnu = rien envoyé ; relecture double, carte désarmée, cohérente avec le 3D de la carte, sans tours ;
  arrêt confirmé après chaque test ; onglet caché = test coupé ; molette bloquée armé ; dialogues refusés sans arrêt relu.
- Fichiers : `src/components/tabs/MotorsTab.vue`, `src/composables/motors/useMotorTesting.js`, `src/js/trueglide/motor_bench.js`.

Firmware `Jschmiedd/Firmware-TrueGlide`, branche `main`, de `0c46083` à `ee050eb` (flashé sur le Mini Pouce) :
- garde MSP par port (`flight/msp_motor_guard.c`) : seul le port qui a lancé le test l'entretient ; arrêt après 1 s ;
- enregistrement EEPROM : moteurs remis à l'arrêt ;
- `drivers/motor.c` : bornes et arrêt calculés avec le protocole du pilote qui tourne (défaut C1 trouvé en 3e relecture) ;
- `fc/core.c` : valeur de test effacée à l'armement.

## Preuves

- Tests logiciel : `test/js/trueglide_onglet_moteurs_monte.test.js` (vrai onglet monté sur une carte DShot simulée :
  le défaut est reproduit sur l'ancien code, absent sur ebee781), `test/js/trueglide_arret_moteurs.test.js`.
- Tests firmware : 76 suites `make test-all` ; `motor_endpoints_unittest` (échoue sans la correction C1) ;
  `msp_motor_guard_unittest` ; SITL `trueglide/sitl_bench/test_msp_motor_safety.py` 7/7.
- Simulateur : partie 5 bis déroulée dans le vrai logiciel, 17/17 (`controle/photos/TG-A1-5B/`).
- Relectures indépendantes : 2e (4 points, corrigés), 3e (C1 critique, corrigé), relecture ciblée de C1 : « oui ».

## Essais sur la vraie carte (Mini Pouce, SpeedyBee F405 Mini, DShot300 bidir, Walksnail)

- TG-A1 complète : toutes les questions « oui » (`controle/remplies/TG-A1_fiche_Johann_2026-11-07.docx`).
- 3D oui / non + Sauvegarder, batterie branchée (24a71eb + ebee781) : aucun mouvement, arrêt 1500 / 1000, RPM 0.
- TG-A1-5C (ee050eb) : test coupé, onglet quitté, USB débranché (arrêt seul en 1 à 2 s) : 3 « oui ».
- Reconnexion USB batterie branchée : OK (vidéo).

## Restes connus (non corrigés, à juger)

- 3D changé sans redémarrage (comportement Betaflight d'origine).
- Molette : fait monter les moteurs en faisant défiler la page au-dessus des curseurs pendant le test (borné à 1300).
- Dialogue « sens de rotation DShot » : pas d'arrêt quand l'onglet est caché.
- Arrêt de la carte inconnu : le logiciel ne peut pas arrêter, consigne = débrancher la batterie.
- /app/ a encore le défaut d'origine (publication sur ordre écrit de Johann seulement).
- Flasheur : « Dernière carte connectée : ? » dans la fenêtre « Carte différente ».

## Question posée à l'audit

Les corrections et les essais suffisent-ils pour publier ebee781 sur /app/ ? Sinon, quoi faire avant ?

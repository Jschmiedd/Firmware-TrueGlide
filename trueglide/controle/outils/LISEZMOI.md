# Outils d'essai au simulateur (SITL) — sécurité du test moteurs (07/11/2026)

À lancer avec le SITL TrueGlide qui écoute sur le port TCP 5761 (`obj/main/trueglide_SITL.elf`).

- `msp_motor_safety.cjs` : envoie MSP_SET_MOTOR puis vérifie (MSP_MOTOR) que la valeur reste pendant le trafic MSP,
  revient à l'arrêt après 1 s sans trame MSP et après MSP_EEPROM_WRITE (firmware 81fa7d6).
- `interrompu.cjs` : simule une sauvegarde interrompue (MSP_SET_FEATURE_CONFIG avec le 3D basculé, sans EEPROM) et
  affiche l'arrêt relu, pour vérifier que le logiciel suit le 3D en marche et non la configuration.

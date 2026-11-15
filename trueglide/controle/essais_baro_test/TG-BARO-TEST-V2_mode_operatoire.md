# Essai TG-BARO-TEST v2 : octets bruts du baromètre (USB seul, Mini Pouce + carte neuve)

Date : …… Fait par : ……

Firmware de TEST (jamais en vol) : `TEST_baro_v2_3b418e8_SPEEDYBEEF405MINI.hex`
(sha256 a17b5ecb…c3b771cb99). Base : Betaflight de la PR #16023 (542c458). Version attendue : « 2026.12.0 (3b418e8dd) ».
Logiciel : BETAFLIGHT APP (app.betaflight.com). Même fichier pour les 2 cartes (SpeedyBee F405 Mini).

Ce que le journal enregistre (debug_mode BARO) : [0] état, [1] pression hPa, [2] température 0,01 °C,
[3] registre MEAS_CFG lu juste avant (bit « mesure prête ») et 2 signes d'erreur de lecture, [4] [5] [6] les 6 octets bruts du capteur, [7] numéro d'échantillon.

But : au moment d'une valeur fausse, savoir si le capteur disait « mesure prête » et ce qu'il a envoyé.
Comparer Mini Pouce avec une carte neuve : si seule Mini Pouce a des valeurs fausses, son capteur devient le premier suspect.

Règles :
1. USB SEUL pendant tout l'essai. Jamais de batterie.
2. Ne jamais voler avec ce firmware.
3. Les sauvegardes `diff all` restent sur le PC (identifiant de la carte) : ne les publier nulle part.

## Phase A. Mini Pouce : sauvegarde et flash
1. USB, « Connexion ». CLI : `version`. La ligne contient « cdd6f3844 » (test v1). Sinon : prévenir.
2. CLI : `diff all`. Copier tout le texte dans `MINI_POUCE_avant_test_v2.txt` sur le PC.
3. « Déconnecter ». « Flasher le Firmware » : « Charger un Firmware [Local] », le .hex v2.
   « Effacement complet de la puce » activé. « Flasher le Firmware ».
4. « Connexion ». CLI : coller tout `MINI_POUCE_avant_test_v2.txt`, Entrée. La carte redémarre.
5. « Connexion ». CLI : `version`, `get debug_mode`, `status`.
- version contient « 3b418e8dd » ? debug_mode = BARO ? status : « BARO: DPS310 » ?
- Aucune ligne ###ERROR au collage ?

## Phase B. Mini Pouce : enregistrement de 5 minutes
1. Onglet « Blackbox » : « Effacer la mémoire interne ».
2. CLI : `set blackbox_sample_rate = 1/16`, puis `set blackbox_mode = ALWAYS`, puis `save`.
3. « Connexion », onglet « Capteurs ». Rester 5 minutes au chronomètre sans toucher (PAS dans la CLI).
4. CLI : `set blackbox_mode = NORMAL`, `save`. « Connexion », CLI : `get blackbox_mode` → NORMAL ?
5. Onglet « Blackbox » : « Enregistrer sous… » → `mini_pouce_v2.bbl`.

## Phase C. Carte neuve : flash et réglage (USB seul, rien de soudé)
1. USB, « Connexion ». CLI : `version`. Noter la ligne (firmware d'usine).
2. « Déconnecter ». Flasher le même .hex v2, « Effacement complet de la puce » activé.
3. « Connexion ». Onglet « Blackbox » : « Effacer la mémoire interne ».
4. CLI : `set debug_mode = BARO`, `set blackbox_sample_rate = 1/16`, `set blackbox_mode = ALWAYS`, `save`.
5. « Connexion ». CLI : `version`, `status`.
- version contient « 3b418e8dd » ? status : « BARO: DPS310 » et une ligne « FLASH: » ?

## Phase D. Carte neuve : enregistrement de 5 minutes
Comme la phase B, étapes 3 à 5 → `carte_neuve_v2.bbl`.

## À envoyer (à la discussion TRUEGLIDE, jamais sur GitHub tel quel)
Les lignes version et status des 2 cartes, les 2 fichiers .bbl, ce qui a surpris.
Après l'essai : Mini Pouce garde le firmware de test (pas de vol) ; on décidera ensuite.

# Essai TG-BARO-TEST : trouver la cause des valeurs fausses du baromètre (banc, Mini Pouce)

Date : …… Fait par : ……

Firmware de TEST (pas pour voler) : `TEST_baro_i2c_cdd6f38_SPEEDYBEEF405MINI.hex`, envoyé par la discussion de l'auditeur
(sha256 65bb73df…cb21). C'est le Betaflight de la PR #16023, plus 3 compteurs dans le journal : erreurs I2C, lectures
utilisées avant la fin, lectures utilisées en erreur. Logiciel : BETAFLIGHT APP (app.betaflight.com).

Règles :
1. HÉLICES RETIRÉES pendant tout l'essai. Drone posé à plat et fixé.
2. Batterie branchée 40 s au plus par passage (VTX), puis débrancher la batterie et attendre 1 minute.
3. Toujours armer en ANGLE (AUX3 bas), gaz en bas.
4. Armement impossible avec l'USB et le logiciel connectés : débrancher l'USB avant de brancher la batterie.
5. Ne pas voler avec ce firmware.

## Phase A. Sauvegarde et flash (dans le logiciel, USB, batterie débranchée)
1. Brancher l'USB. « Connexion ». Onglet « Ligne de commande (CLI) » : `version`, Entrée. La ligne doit contenir
   « 2026.12.0 (542c45840) » (firmware de la PR). Si ce n'est pas le cas (par exemple TRUEGLIDE) : ne pas continuer,
   prévenir.
2. CLI : `diff all`, Entrée. Copier tout le texte dans `MINI_POUCE_avant_test_baro.txt` sur le PC. Ne le publier nulle
   part (il contient l'identifiant de la carte).
3. « Déconnecter ». Onglet « Flasher le Firmware » : petite flèche à droite du bouton jaune « Charger un Firmware
   [Online] », choisir « Charger un Firmware [Local] », puis le .hex de test.
4. Vérifier que « Effacement complet de la puce » est activé. « Flasher le Firmware ».
5. « Connexion ». CLI : coller tout `MINI_POUCE_avant_test_baro.txt`, Entrée. La carte redémarre.
6. « Connexion ». CLI : `version`, `get debug_mode`, `get osd_displayport_device`.
- version contient « 2026.12.0 (cdd6f3844) » ?
- debug_mode = BARO et osd_displayport_device = MSP ? (Sinon : prévenir, ne pas continuer.)
- Aucune ligne ###ERROR au collage ?

## Phase B. Témoin sur USB seul (dans le logiciel)
1. Onglet « Blackbox » : « Effacer la mémoire interne ».
2. CLI : `set blackbox_mode = ALWAYS` puis `save`.
3. Rester connecté 2 minutes sans rien toucher.
4. CLI : `set blackbox_mode = NORMAL` puis `save`.
5. CLI : `get blackbox_mode` → NORMAL ?

## Phase C. Séance 1 : 2 passages armé au ralenti (avec la radio, HÉLICES RETIRÉES)
1. « Déconnecter », débrancher l'USB. Radio allumée.
2. Passage 1 : brancher la batterie, armer en ANGLE, gaz en bas. Compter 30 s. Désarmer. Débrancher la batterie.
   Attendre 1 minute.
3. Passage 2 : pareil.
4. USB (batterie débranchée), « Connexion », onglet « Blackbox » : « Enregistrer sous... » → `test_baro_seance1.bbl`.

## Phase D. Séance 2 : 2 passages, plus tard (avec la radio, HÉLICES RETIRÉES)
Comme la phase C, sans effacer la mémoire → `test_baro_seance2.bbl`.

## À envoyer (à la discussion TRUEGLIDE, jamais sur GitHub tel quel)
Les lignes version et get, les 2 fichiers .bbl, ce qui a surpris. Après l'essai, le firmware reste celui de test,
on décidera ensuite.

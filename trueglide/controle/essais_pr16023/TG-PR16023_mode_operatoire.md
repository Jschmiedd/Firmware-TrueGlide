# Essai TG-PR16023 : vol avec le firmware de la PR Betaflight #16023 (I2C), puis ALT HOLD (Mini Pouce)

Date : …… Lieu : …… Fait par : ……

**Plan**
- Phase A : flasher le firmware de la PR, dans Betaflight App (USB, batterie débranchée).
- Phase B : remettre tes réglages et préparer l'enregistrement, dans Betaflight App.
- Phase C : failsafe au sol, avec la radio (**hélices retirées**).
- Phase D : vol normal, puis ALT HOLD, avec la radio (hélices montées).
- Phase E : relevés après le vol, dans Betaflight App.
- Phase F : retour en TRUEGLIDE cb4e3f0, dans TRUEGLIDE /app/.

Pourquoi : Betaflight demande le journal Blackbox d'un **vrai vol avec le firmware de la PR** avant de la passer en
« ready ». À faire **après** TG-P3B (TG-P3B doit être OUI). C'est un firmware Betaflight **en développement** (master),
pas TRUEGLIDE : aucune protection TRUEGLIDE pendant ce vol.

Mêmes règles que TG-P3B :
1. **Toujours armer en ANGLE (AUX3 bas).** Armé en ACRO, LAUNCH CONTROL (sur AUX1) s'active.
2. **VTX : batterie branchée au sol 45 s au plus, à chaque fois.**
3. Armement impossible avec l'USB et le logiciel connectés : brancher la batterie, débrancher l'USB, puis armer.

## Phase A. Flasher le firmware de la PR (dans Betaflight App)

1. Batterie débranchée. Brancher l'USB. Ouvrir **app.betaflight.com**.
2. Activer le **mode expert** (interrupteur en haut de l'écran).
3. Onglet « Flasher le firmware » : carte **SPEEDYBEEF405MINI**.
4. Afficher les versions de développement, puis dans le champ **« Select commit »** taper **#16023**.
5. Options de construction : cocher **GPS** (sans GPS, ALT HOLD n'est pas dans le firmware), en plus des options
   proposées par défaut (récepteur CRSF, OSD, télémétrie).
6. **Effacement complet** activé. Flasher.
7. « Connexion ». Onglet CLI : taper `version`, Entrée. Noter toute la ligne.

- La ligne de `version` contient « Betaflight », « SPEEDYBEEF405MINI » et un numéro de commit (à noter) ?

## Phase B. Remettre les réglages et préparer l'enregistrement (dans Betaflight App)

1. Onglet CLI : coller tout le fichier `mini_pouce_betaflight_pr16023.txt`, Entrée. La carte redémarre.
2. « Connexion ». Onglet CLI : taper `diff all`, Entrée. Copier tout le texte (à envoyer).
3. Taper `set debug_mode = BARO`, Entrée. Taper `save`, Entrée. La carte redémarre.
4. « Connexion ». Onglet Modes : vérifier ARM sur AUX1, ANGLE sur AUX3 bas, ALTHOLD sur AUX4 haut.
5. Onglet Blackbox : « Effacer la mémoire flash ».
6. Onglet CLI : taper `status`, Entrée. Noter la ligne « I2C=… (… errors) » et la ligne du gyro.
7. « Déconnecter ». Débrancher l'USB.

- Au collage (étape 1), aucune ligne `###ERROR` ? (S'il y en a : noter les lignes, **ne pas voler**, me les envoyer.)
- Les 3 modes sont sur les bons interrupteurs ?

## Phase C. Failsafe au sol (avec la radio, HÉLICES RETIRÉES)

Cet essai prouve l'arrêt au sol (gaz en bas). Il ne prouve pas le DROP en vol.

1. Radio allumée. Brancher la batterie. Drone posé à plat, fixé.
2. Avec la radio, AUX3 en bas (ANGLE) : armer, gaz en bas. Les moteurs tournent au ralenti.
3. Éteindre la radio.
4. Compter : les moteurs doivent s'arrêter en 2 s environ.
5. Remettre l'interrupteur ARM sur désarmé. Rallumer la radio.
6. Débrancher la batterie.

- À l'étape 4, les moteurs se sont arrêtés seuls ?
- Si NON : **ne pas voler**, me prévenir.

## Phase D. Vol normal puis ALT HOLD (avec la radio, hélices montées)

Terrain dégagé, personne autour. Batterie pleine.

1. Monter les hélices. Brancher la batterie, drone posé à plat.
2. Avec la radio, en ANGLE : armer, décoller, stationnaire à 2–3 m, **1 minute**. Noter le % de gaz du stationnaire.
3. Quelques virages doux.
4. Monter à **3 m au moins**, stationnaire stable.
5. Basculer AUX4 en haut (ALT HOLD). S'attendre à ce que le drone descende ou monte pendant 1 à 2 s : garder 3 m au
   moins et le doigt sur AUX4.
6. Ne plus toucher les gaz. Regarder **10 secondes**.
7. Si le drone monte, descend ou saute : AUX4 en bas tout de suite, reprendre aux gaz.
8. AUX4 en bas. Reprendre la main aux gaz.
9. Recommencer les étapes 5 à 8 une fois.
10. Atterrir, désarmer. **Garder la batterie branchée** (le compteur I2C repart à zéro sans courant).

- Le drone a volé comme d'habitude (pas de vibration, pas d'oscillation, pas de bruit nouveau) ?
- ALT HOLD : le drone a gardé sa hauteur (à 1 m près) pendant les 10 s ?
- Aucun saut brusque de hauteur ?

## Phase E. Relevés après le vol (dans Betaflight App)

Faire les étapes 1 et 2 dans les 45 s après l'atterrissage ; sinon débrancher la batterie (la ligne I2C ne sera pas
relevée).

1. Batterie toujours branchée. Brancher l'USB, « Connexion ».
2. Onglet CLI : taper `status`, Entrée. Copier tout le texte.
3. Débrancher la batterie.
4. Onglet Blackbox : « Enregistrer » le journal. **Renommer le fichier en `.log`** (GitHub refuse `.bbl`).

- La ligne I2C indique 0 erreur ? (Noter le nombre, même s'il n'est pas 0.)

## Phase F. Retour en TRUEGLIDE cb4e3f0 (dans TRUEGLIDE /app/)

1. Batterie débranchée. TRUEGLIDE /app/, « Flasher le firmware » : SPEEDYBEEF405MINI, version **cb4e3f0**,
   **effacement complet**, flasher.
2. « Connexion », CLI : coller `MINI_POUCE_TRUEGLIDE_diff_all_2026-11-08_cb4e3f0.txt`, Entrée.
3. « Connexion », CLI : `version`, puis `get craft_name`.

- `version` contient cb4e3f0 et `craft_name` = MINI POUCE ?

## À envoyer

1. La ligne `version` (phase A) et le `diff all` (phase B).
2. Les deux textes `status` (phases B et E).
3. Le journal Blackbox renommé en `.log`.
4. Le % de gaz du stationnaire.
5. La vidéo des lunettes (DVR), surtout l'ALT HOLD.
6. Ce qui t'a surpris.

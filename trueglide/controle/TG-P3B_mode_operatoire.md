# Essai TG-P3B : premier vol de TRUEGLIDE cb4e3f0 (Mini Pouce), sans ALT HOLD

Date : …… Lieu : …… Fait par : ……

**Plan**
- Phase A : préparation, dans le logiciel (USB, batterie débranchée).
- Phase B : failsafe au sol, avec la radio (**hélices retirées**).
- Phase C : vol normal, avec la radio (hélices montées).
- Phase D : après le vol, dans le logiciel.

Pas d'ALT HOLD dans ce vol : il se fera plus tard, sur une fiche à part, avec `ap_hover_throttle` réglé d'après le % de
gaz noté en phase C.

Ce qu'on vérifie : `cb4e3f0` vole comme avant, le failsafe coupe les moteurs, le baromètre ne donne pas de valeur fausse.
Rien d'autre n'est changé dans la configuration (sauvegarde du 08/11), sauf l'enregistrement du baromètre.
Les fonctions TRUEGLIDE de vol (anti-surchauffe, relance moteur, Speeder) restent **désactivées** : elles seront
essayées plus tard (TG-P3C).

Interrupteurs du Mini Pouce (sauvegarde du 08/11) : ARM sur AUX1 ; AUX3 bas = ANGLE, milieu = HORIZON, haut = ACRO ;
ALT HOLD sur AUX4 en haut (**non utilisé dans ce vol**). Failsafe : réglage d'origine (DROP).

**Toujours armer en ANGLE (AUX3 bas) : armé en ACRO, LAUNCH CONTROL (sur AUX1) s'active.**

**VTX : batterie branchée au sol 45 s au plus, à chaque fois.**

## Phase A. Préparation (dans le logiciel)

1. Batterie débranchée. Brancher l'USB. TRUEGLIDE /app/, « Connexion ».
2. Tableau de bord, case Firmware : lire « TRUEGLIDE 2026.6.2 (cb4e3f0) ».
3. Tableau de bord, case « Fonctions TrueGlide » : tout sur « Désactivé ».
4. Onglet Blackbox : « Effacer la mémoire flash ».
5. Onglet CLI : taper `set debug_mode = BARO`, Entrée.
6. Taper `save`, Entrée. La carte redémarre.
7. « Connexion », onglet CLI : taper `status`, Entrée. Noter la ligne « I2C=… (… errors) ».
8. « Déconnecter ». Débrancher l'USB.

- Firmware cb4e3f0 et fonctions TrueGlide toutes « Désactivé » ?

## Phase B. Failsafe au sol (avec la radio, HÉLICES RETIRÉES)

Cet essai prouve l'arrêt au sol (gaz en bas). Il ne prouve pas le DROP en vol.

Drone posé à plat, fixé. Lunettes allumées. **45 s au plus** batterie branchée (le VTX chauffe).

1. Radio allumée. Brancher la batterie.
2. Avec la radio, AUX3 en bas (ANGLE) : armer, gaz en bas. Les moteurs tournent au ralenti.
3. Éteindre la radio.
4. Compter : les moteurs doivent s'arrêter en 2 s environ.
5. Remettre l'interrupteur ARM sur désarmé. Rallumer la radio.
6. Débrancher la batterie.

- À l'étape 4, les moteurs se sont arrêtés seuls ? (Noter si les lunettes ont affiché « FAILSAFE ».)
- Si NON : **ne pas voler**, m'envoyer la vidéo des lunettes.

## Phase C. Vol normal (avec la radio, hélices montées)

Terrain dégagé, personne autour. Batterie pleine. **Ne pas toucher AUX4 pendant tout le vol.**

1. Monter les hélices. Brancher la batterie, drone posé à plat.
2. Avec la radio, en ANGLE : armer, décoller, stationnaire à 2–3 m, **1 minute**. Noter le % de gaz affiché dans
   les lunettes pendant le stationnaire.
3. Quelques virages doux, puis 1 minute dans ton mode habituel.
4. Atterrir, désarmer. **Garder la batterie branchée** (le compteur I2C repart à zéro sans courant).

- Le drone vole comme avec Betaflight (pas de vibration, pas d'oscillation, pas de bruit nouveau) ?
- Moteurs tièdes après le vol (pas brûlants) ?

## Phase D. Après le vol (dans le logiciel)

Faire les étapes 1 et 2 dans les 45 s après l'atterrissage ; sinon débrancher la batterie (la ligne I2C ne sera pas
relevée).

1. Batterie toujours branchée. Brancher l'USB, « Connexion ».
2. Onglet CLI : taper `status`, Entrée. Noter la ligne « I2C=… (… errors) ».
3. Débrancher la batterie.
4. Onglet Blackbox : « Enregistrer » le journal. Noter le nom du fichier.

- La ligne I2C est notée ? (Ce compteur compte seulement les blocages du bus, pas toutes les erreurs : 0 ne prouve pas
  à lui seul qu'il n'y a eu aucune erreur ; le journal Blackbox le dira.)

## À envoyer

1. Le journal Blackbox (phase E).
2. Les deux lignes I2C (phase A = référence, phase D = vol).
3. Le % de gaz du stationnaire (phase C).
4. La vidéo des lunettes (DVR).
5. Ce qui t'a surpris.

# Essai TG-P3B : premier vol de TRUEGLIDE cb4e3f0, puis ALT HOLD (Mini Pouce)

Date : …… Lieu : …… Fait par : ……

**Plan**
- Phase A : préparation, dans le logiciel (USB, batterie débranchée).
- Phase B : failsafe au sol, avec la radio (**hélices retirées**).
- Phase C : vol normal, avec la radio (hélices montées).
- Phase D : ALT HOLD, avec la radio.
- Phase E : après le vol, dans le logiciel.

Ce qu'on vérifie : `cb4e3f0` vole comme avant, le failsafe coupe les moteurs, ALT HOLD tient la hauteur sans saut.
Rien d'autre n'est changé dans la configuration (sauvegarde du 08/11), sauf l'enregistrement du baromètre.
Les fonctions TRUEGLIDE de vol (anti-surchauffe, relance moteur, Speeder) restent **désactivées** : elles seront
essayées plus tard (TG-P3C).

Interrupteurs du Mini Pouce (sauvegarde du 08/11) : ARM sur AUX1 ; AUX3 bas = ANGLE, milieu = HORIZON, haut = ACRO ;
ALT HOLD sur AUX4 en haut. Failsafe : réglage d'origine (DROP : la carte désarme 1,5 s après la perte de la radio).

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

Drone posé à plat, fixé. Lunettes allumées. **45 s au plus** batterie branchée (le VTX chauffe).

1. Radio allumée. Brancher la batterie.
2. Avec la radio : armer, gaz en bas. Les moteurs tournent au ralenti.
3. Éteindre la radio.
4. Compter : les moteurs doivent s'arrêter en 2 s environ.
5. Remettre l'interrupteur ARM sur désarmé. Rallumer la radio.
6. Débrancher la batterie.

- À l'étape 4, les moteurs se sont arrêtés seuls, et les lunettes ont affiché « FAILSAFE » ?
- Si NON : **ne pas voler**, m'envoyer la vidéo des lunettes.

## Phase C. Vol normal (avec la radio, hélices montées)

Terrain dégagé, personne autour. Batterie pleine.

1. Monter les hélices. Brancher la batterie, drone posé à plat.
2. Avec la radio, en ANGLE : armer, décoller, stationnaire à 2–3 m, **1 minute**.
3. Quelques virages doux, puis 1 minute dans ton mode habituel.
4. Atterrir, désarmer. **Garder la batterie branchée** si elle a encore de la charge pour la phase D (sinon, batterie
   pleine pour la phase D, et la ligne I2C de la phase E ne compte que la phase D).

- Le drone vole comme avec Betaflight (pas de vibration, pas d'oscillation, pas de bruit nouveau) ?
- Moteurs tièdes après le vol (pas brûlants) ?

## Phase D. ALT HOLD (avec la radio)

ALT HOLD tient la hauteur avec le baromètre. Le manche des gaz au milieu = garder la hauteur.

1. Avec la radio, en ANGLE : décoller, stationnaire stable à **3 m au moins**.
2. Gaz à la position du stationnaire. Basculer AUX4 en haut (ALT HOLD).
3. Ne plus toucher les gaz. Regarder **10 secondes** : le drone reste à la même hauteur ?
4. Si le drone monte, descend ou saute : AUX4 en bas tout de suite, reprendre aux gaz.
5. AUX4 en bas. Reprendre la main aux gaz, stationnaire.
6. Recommencer les étapes 2 à 5 une fois.
7. Atterrir, désarmer. **Garder la batterie branchée** (le compteur I2C repart à zéro sans courant).

- ALT HOLD : le drone a gardé sa hauteur (à 1 m près) pendant les 10 s ?
- Aucun saut brusque de hauteur, ni vers le haut ni vers le bas ?
- AUX4 en bas : la main est revenue tout de suite aux gaz ?

## Phase E. Après le vol (dans le logiciel)

1. Batterie toujours branchée. Brancher l'USB, « Connexion ».
2. Onglet CLI : taper `status`, Entrée. Noter la ligne « I2C=… (… errors) ».
3. Débrancher la batterie.
4. Onglet Blackbox : « Enregistrer » le journal. Noter le nom du fichier.

- La ligne I2C indique 0 erreur ?

## À envoyer

1. Le journal Blackbox (phase E).
2. Les deux lignes I2C (phases A et E).
3. La vidéo des lunettes (DVR), surtout la phase D.
4. Ce qui t'a surpris.

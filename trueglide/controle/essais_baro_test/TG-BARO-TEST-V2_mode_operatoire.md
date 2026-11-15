# Essai TG-BARO-TEST v2 : octets bruts du baromètre (USB seul, Mini Pouce + carte neuve)

Date : …… Fait par : ……

**Plan**
- Phase A : Mini Pouce, sauvegarde et flash du firmware de test, dans le logiciel (Betaflight App).
- Phase B : Mini Pouce, enregistrement de 5 minutes, dans le logiciel.
- Phase C : carte neuve, sauvegarde, flash et réglage, dans le logiciel.
- Phase D : carte neuve, enregistrement de 5 minutes, dans le logiciel.

Firmware de TEST (jamais en vol) : `TEST_baro_v2_3b418e8_SPEEDYBEEF405MINI.hex`
(sha256 a17b5ecba8aff817f50ce62bad2945cf368dee94328b7a91ee7d87c3b771cb99). Base : Betaflight de la PR #16023
(542c458). Version attendue : « 2026.12.0 (3b418e8dd) ». Logiciel : BETAFLIGHT APP (app.betaflight.com).
Même fichier pour les 2 cartes (SpeedyBee F405 Mini).

Ce que le journal enregistre (debug_mode BARO) : [0] état, [1] pression hPa, [2] température 0,01 °C,
[3] registre MEAS_CFG lu juste avant (bit « mesure prête ») et 2 signes d'erreur de lecture, [4] [5] [6] les 6 octets
bruts du capteur, [7] numéro d'échantillon.

But : au moment d'une valeur fausse, savoir si le capteur disait « mesure prête » et ce qu'il a envoyé. Comparer
Mini Pouce avec une carte neuve : si seule Mini Pouce a des valeurs fausses, son capteur devient le premier suspect.

Règles :
1. USB SEUL pendant tout l'essai. Jamais de batterie.
2. Ne jamais voler avec ce firmware.
3. Les sauvegardes `diff all` restent sur le PC, hors de tout dépôt (identifiant de la carte) : ne les publier nulle part.
4. Avant de flasher, vérifier le fichier, dans PowerShell : `Get-FileHash <fichier .hex>`. Le résultat s'affiche en
   MAJUSCULES : seuls les caractères comptent, pas la casse. Il doit être exactement le sha256 ci-dessus. Sinon : ne pas
   flasher, prévenir.
5. Si `status` n'affiche pas « BARO: DPS310 » : arrêter l'essai sur cette carte, prévenir.
6. Chaque enregistrement : mémoire Blackbox effacée juste avant, 5 minutes, jamais plus de 10. (Témoin du 15/11 :
   P interval 4, boucle PID à 4 kHz, soit 1000 cadres par seconde, 7,8 Mo en 189 s. Avec 1/16 : 250 cadres par
   seconde, mémoire de 8 Mo pleine vers 12 minutes.)
7. Les journaux ne sont rangés dans un dépôt que masqués (ligne « H DeviceUID » retirée).
8. Onglet Blackbox : je n'ai pas encore de capture de cet onglet. Quand tu y arrives la première fois, envoie-moi une
   capture : je t'indique où cliquer avant que tu cliques.

## Phase A. Mini Pouce : sauvegarde et flash (dans le logiciel)

1. Dans le logiciel : brancher l'USB, cliquer « Connecter ».
2. Dans le logiciel : onglet « Ligne de commande (CLI) », taper `version`, Entrée.
3. Dans le logiciel : taper `diff all`, Entrée. Copier tout le texte dans `MINI_POUCE_avant_test_v2.txt` sur le PC.
4. Dans le logiciel : cliquer « Déconnecter », puis onglet « Flasher le Firmware ».
5. Dans le logiciel : activer « Effacement complet de la puce » (interrupteur jaune).
6. Dans le logiciel : petite flèche à droite du bouton jaune « Charger un Firmware [Online] », choisir
   « Charger un Firmware [Local] », puis le fichier .hex v2.
7. Dans le logiciel : cliquer « Flasher le Firmware ». Attendre la fin.
8. Dans le logiciel : cliquer « Connecter ». Onglet CLI : coller tout `MINI_POUCE_avant_test_v2.txt`, Entrée.
   La carte redémarre.
9. Dans le logiciel : cliquer « Connecter ». Onglet CLI : taper `version`, Entrée.
10. Dans le logiciel : taper `get debug_mode`, Entrée.
11. Dans le logiciel : taper `status`, Entrée.

- À l'étape 2, la ligne contient « cdd6f3844 » (test v1) ?
- Le sha256 du fichier (règle 4) est le bon ?
- Au collage (étape 8), aucune ligne `###ERROR` ?
- À l'étape 9, la ligne contient « 3b418e8dd » ?
- À l'étape 10, debug_mode = BARO ?
- À l'étape 11, « BARO: DPS310 » (sinon : arrêt) ?

![Onglet « Flasher le Firmware » : 1. effacement complet activé ; 2. petite flèche, « Charger un Firmware [Local] » ; 3. « Flasher le Firmware ».](flasher_local_effacement.jpg)

## Phase B. Mini Pouce : enregistrement de 5 minutes (dans le logiciel)

1. Dans le logiciel : onglet « Blackbox », cliquer « Effacer la mémoire interne ».
2. Dans le logiciel : confirmer avec « Oui, effacer la mémoire interne ». Attendre la fin.
3. Dans le logiciel : onglet CLI, taper `set blackbox_sample_rate = 1/16`, Entrée.
4. Dans le logiciel : taper `set blackbox_mode = ALWAYS`, Entrée.
5. Dans le logiciel : taper `save`, Entrée. La carte redémarre.
6. Dans le logiciel : cliquer « Connecter », puis onglet « Capteurs ».
7. Rester 5 minutes au chronomètre sur l'onglet « Capteurs », sans toucher (PAS dans la CLI).
8. Dans le logiciel : onglet CLI, taper `set blackbox_mode = NORMAL`, Entrée, puis `save`, Entrée.
9. Dans le logiciel : cliquer « Connecter ». Onglet CLI : taper `get blackbox_mode`, Entrée.
10. Dans le logiciel : onglet « Blackbox », cliquer « Enregistrer sous... ».
11. Dans la fenêtre de Windows : choisir le dossier, nommer le fichier `mini_pouce_v2.bbl`, cliquer « Enregistrer ».

- À l'étape 7, 5 minutes au chronomètre (pas plus de 10) ?
- À l'étape 9, blackbox_mode = NORMAL ?
- Le fichier `mini_pouce_v2.bbl` est enregistré ?

## Phase C. Carte neuve : sauvegarde, flash et réglage (dans le logiciel, USB seul, rien de soudé)

1. Dans le logiciel : débrancher Mini Pouce, brancher la carte neuve en USB, cliquer « Connecter ».
2. Dans le logiciel : onglet CLI, taper `version`, Entrée. Noter la ligne (firmware d'usine).
3. Dans le logiciel : taper `diff all`, Entrée. Copier tout le texte dans `CARTE_NEUVE_usine.txt` sur le PC.
4. Dans le logiciel : cliquer « Déconnecter », onglet « Flasher le Firmware ».
5. Dans le logiciel : activer « Effacement complet de la puce », choisir « Charger un Firmware [Local] », le même
   .hex v2, cliquer « Flasher le Firmware ». Attendre la fin.
6. Dans le logiciel : cliquer « Connecter ». Onglet « Blackbox » : « Effacer la mémoire interne », puis
   « Oui, effacer la mémoire interne ».
7. Dans le logiciel : onglet CLI, taper `set debug_mode = BARO`, Entrée.
8. Dans le logiciel : taper `set blackbox_sample_rate = 1/16`, Entrée.
9. Dans le logiciel : taper `set blackbox_mode = ALWAYS`, Entrée.
10. Dans le logiciel : taper `save`, Entrée. La carte redémarre.
11. Dans le logiciel : cliquer « Connecter ». Onglet CLI : taper `version`, Entrée.
12. Dans le logiciel : taper `status`, Entrée.

- À l'étape 11, la ligne contient « 3b418e8dd » ?
- À l'étape 12, « BARO: DPS310 » (sinon : arrêt) ?
- À l'étape 12, une ligne « FLASH: » ?

## Phase D. Carte neuve : enregistrement de 5 minutes (dans le logiciel)

1. Dans le logiciel : onglet « Capteurs ». (L'enregistrement a commencé au redémarrage de l'étape C.10.)
2. Rester 5 minutes au chronomètre sur l'onglet « Capteurs », sans toucher (PAS dans la CLI).
3. Dans le logiciel : onglet CLI, taper `set blackbox_mode = NORMAL`, Entrée, puis `save`, Entrée.
4. Dans le logiciel : cliquer « Connecter ». Onglet CLI : taper `get blackbox_mode`, Entrée.
5. Dans le logiciel : onglet « Blackbox », cliquer « Enregistrer sous... ».
6. Dans la fenêtre de Windows : choisir le dossier, nommer le fichier `carte_neuve_v2.bbl`, cliquer « Enregistrer ».

- À l'étape 2, 5 minutes au chronomètre (pas plus de 10) ?
- À l'étape 4, blackbox_mode = NORMAL ?
- Le fichier `carte_neuve_v2.bbl` est enregistré ?

## À envoyer

### Lignes version et status des 2 cartes

### Ce qui a surpris [grand]

Les 2 fichiers .bbl : à la discussion TRUEGLIDE, jamais sur GitHub tel quel. Après l'essai : Mini Pouce garde le
firmware de test et ne vole pas. Pour voler à nouveau : retour à TRUEGLIDE cb4e3f0 (effacement complet), puis collage
du fichier complet « PID doux + anti-desync », avec sa propre fiche. La carte neuve garde le firmware de test (elle
n'est montée sur aucun drone).

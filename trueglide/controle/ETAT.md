# État de TRUEGLIDE — à lire en premier dans chaque discussion (développement et audit)

Mis à jour le 16/11/2026 (PR Betaflight, essai de la garde TXE au banc). Modes d'emploi (hors ligne, mise en route depuis Betaflight, Studio, plan de vol) corrigés le 07/11 et donnés à Johann (hors dépôt). À mettre à jour à chaque changement (version en ligne, règle, défaut, audit, essai).
Détail et historique : `trueglide/REPRISE.md` et `trueglide/controle/RESULTATS.md`.

## 1. En ligne aujourd'hui

| Quoi | Version | Commit | Date |
|---|---|---|---|
| Logiciel /app/ | 7dc6865 (copie exacte de /test/, main-BauRBTPI.js : onglet ESC v24 ; essai de coupure de Johann réussi, sa décision « OUI, IL FAUT TOUT METTRE À JOUR, PUBLIER SUR TEST ET APP », OUI de l'audit) | gh-pages 9fb42b9 ; configurateur main = version/app-2026-11-08 = essai-onglets = 7dc6865 | 10/11/2026 |
| Logiciel /test/ | 25f82b1 (= 7dc6865 + onglet ESC v25 f646695, OUI de l'audit pour /test/ seulement) | gh-pages 797de81 | 11/11/2026 |
| Firmwares en ligne (catalogue de /app/ et /test/) | **cb4e3f0f2** (code = 5e246cc), 33 cartes, modèle TRUEGLIDE, version d'essai (seule SPEEDYBEEF405MINI essayée sur une vraie carte, révisions précédentes ; cb4e3f0 en essai TG-A1-7) ; précédente 4b3d47cba | gh-pages 674ceca (notes, flasheur, accueil) | 08/11/2026 |
| Onglet ESC | /app/ : onglet/v24 (34244fe) ; /test/ : onglet/v25 (f646695 : ESC en FLASH*FAILED sauté à l'écriture), /app/ après essai au banc et « OUI V25 SUR APP » de Johann | TrueGlide-ESC-Configurator | 11/11/2026 |
| Firmware sur le Mini Pouce de Johann | **À vérifier par `version`** : TRUEGLIDE cb4e3f0 remis le 15/11 (SYNTHESE_BARO), puis vol du 16/11 au matin avec le Betaflight de la PR #16023 (d'après la passation, non relu). Avant tout essai TRUEGLIDE : retour cb4e3f0 + sauvegarde TRUEGLIDE du 08/11 | — | 16/11/2026 |
| Carte neuve (SpeedyBee F405 Mini, banc, montée sur aucun drone) | TRUEGLIDE cb4e3f0f2 (version et `diff all bare` relus le 16/11) + BANC_reglages_carte_neuve.txt (debug BARO, blackbox ALWAYS 1/16) | — | 16/11/2026 |

## 2. Règles permanentes de Johann

- **Format imposé pour TOUS les essais (Johann, 13/11)** : découper en **phases** nommées (Phase A, B, C…) avec un titre qui dit où on va ; dans chaque phase, des étapes **numérotées** (1, 2, 3…), jamais des puces ; une action par ligne, courte ; donner d'abord le plan des phases, puis avancer phase par phase ; image annotée quand c'est utile ; pas de pavés de texte ; dire « avec la radio » ou « dans le logiciel » à chaque fois ; ne pas répondre aux contrôles automatiques par de longs messages.
- **Jamais l'identifiant de la carte** (`mcu_id`, `signature`) dans un fichier ou un texte du dépôt (public). Historique laissé tel quel (décision de Johann, 14/11).
  Générateurs : retirent mcu_id et signature. Avant chaque commit : rechercher « mcu_id » et « signature » dans les fichiers ajoutés, ne pas pousser s'il y en a. Pour partager des réglages : `diff all bare`. Rechercher aussi « DeviceUID » (en-tête Blackbox) ; sur GitHub, seulement le journal masqué, renommé en .log.
- **Règles du 14/11 (Johann)** : question ≠ ordre, identifiant de carte, contributions externes, essais en direct, contrôles de l'auditeur : voir `trueglide/REPRISE.md`, section « Règles ».
- **Banc, armé avec la radio (Johann)** : le VTX chauffe sans air, jamais plusieurs minutes d'affilée. Essais armés au banc : **30 s environ par passage**, pause entre deux passages (comme TG-P2C, 31,7 s).

- 07/11 : « PUBLIE CORRIGE MET A JOUR JE VEUX PLUS D EWRREUR DANS MON LOGICIEL » et, dans la discussion de
  développement, « OUI CORRECTION MISE A JOUR AUDIT ». Sens : corriger, mettre sur /test/, faire passer l'audit
  indépendant ; sur OUI, publier le **logiciel** sur /app/ (et avancer main) sans redemander. Johann n'est sollicité que
  pour un essai au banc indispensable, en une ligne.
- 07/11 : « CA DOIT ETRE CHANGé AUTOMATIQUEMENT SANS MON ACCORD C EST FINI TU METS TOUT A JOUR » (firmwares), puis
  « ALORS ARRETE LE FIRMWARE », puis (07/11, annule l'arrêt) « NON ON REPRENDS LA COMPILATION CAR LE CONTROL AUDIT ESC
  SE FAIT VIA LE LOGICIEL ET PAS LE FIRMWARE » et ici « TU PEUX COMPILER ET FINIR LES MISE A JOUR POUR EN FINIR ? ».
  Sens : firmwares (.hex, index.json) comme le logiciel : compilés, publiés d'abord sur `firmware-test/`, audit ; sur
  OUI, catalogue `firmware/` mis à jour sans redemander, notes de version « version d'essai », seul le F405 Mini essayé
  sur une vraie carte.
- 10/11 : deux mises à jour TRUEGLIDE par an, 1 à 2 mois après chaque Betaflight (juin, décembre) : procédure
  `trueglide/MISE_A_JOUR_BETAFLIGHT.md`. Prochaine : TRUEGLIDE 2026.12 (février 2027).
- 11/11 : tout contrôler avant de proposer quoi que ce soit à Johann (contexte, liens, cohérence, pièces jointes) et donner une version finale du premier coup : pas de corrections en plusieurs fois.
- 14/11 : nouvel auditeur indépendant : discussion session_01VvqUCsZQ7mCBZTnmA6F2FT. Les anciennes (session_01VvqUCt5QANL2A4NKbRdzyx, session_01VvqUCsD9iRLVMzii8FW6NM) ne donnent plus d'OK. Passation : PASSATION_AUDIT_2026-11-14.md.
- 17/11 : **exception unique** à la règle salle blanche du 27/10 (REPRISE, Décisions), décidée par Johann : les 2 fichiers `trueglide/controle/desync/MINI_POUCE_TRUEGLIDE_complet_2026-11-14_PID_doux_anti_desync_cb4e3f0.txt` et `trueglide/controle/pid_doux/MINI_POUCE_TRUEGLIDE_PID_doux_cb4e3f0.txt` (PID et rates venant d'XPRO) restent dans le dépôt. Ses mots : « pas grave c est juste une base rien de grave elle va changer », « c est une exception mais ca sera la seul ». Aucune autre valeur XPRO n'entre dans le dépôt.
- 18/11 : discussions en cours : « développement TRUEGLIDE » ; « demande de matériel » (mails aux fabricants ; elle transfère sans demander le message de Johann au contact qu'indique une réponse automatique, et l'en informe ensuite) ; « préparation de l'appel Orqa » du 20/11. Organisation : REPRISE.md, Règles, « Organisation (18/11) ».
- Toujours : essais sans hélices ; aucun secret affiché ; rien d'un autre produit dans TRUEGLIDE ; commits avec
  `[skip ci]`, sans identifiant de modèle d'IA ; un .hex donné à Johann garde son nom standard ; vérifier la révision
  dans le fichier avant de le donner.

## 2 bis. Signalements à Betaflight : ordre imposé (décision de Johann, 13/11)

Opportunité de se faire connaître : **dans le bon ordre et sans erreur**. Aucune étape sautée, aucune envoyée avant
la précédente.

1. Audit de la fiche TG-B1 (OUI), puis essai TG-B1 par Johann, filmé (écran et moteurs).
2. Vérification des vidéos et du journal : chaque défaut reproduit sous Betaflight App, chaque protection vue sous
   TRUEGLIDE. Un défaut non reproduit n'est pas signalé.
3. Re-vérification sur le code amont le plus récent (master) le jour de la rédaction : déjà corrigé = pas signalé ;
   recherche des doublons (issues et PR).
4. Rédaction en anglais, un signalement par défaut, neutre et technique, IA déclarée (règle Betaflight), une phrase
   « trouvé en développant TRUEGLIDE ».
5. Relecture et OUI de l'audit, puis OUI écrit de Johann. Envoi par Johann depuis son compte GitHub.
6. Seulement après les réponses de Betaflight : vidéo publique TalkTrueFPV.

Candidats (13/11) : pilote I2C sans remise à zéro après erreur (prouvé au banc le 13/11 par injection, A/B) ; intensité MSP_BATTERY_STATE lue sans signe ; GM3
`&&` ; molette ; page cachée. Pas de confirmation de betaflight#15533 : le Mini Pouce n'utilise pas le DShot bitbang.

État des PR de Johann au 17/11 au matin (preuves : emails GitHub et captures envoyés par Johann ; auditeur pour #5791) :
- betaflight-configurator **#5789** (MSP_BATTERY_STATE signé) : **fusionnée** dans master le 16/11 (haslinghuis), approuvée par blckmn, haslinghuis, nerdCopter ; RN: BUGFIX, projet et jalon 2026.12.
- betaflight-configurator **#5790** (molette) : **fusionnée** dans master le 17/11 à 08 h 58 (blckmn, email « Merged #5790 into master »), approuvée par haslinghuis et blckmn ; nerdCopter préfère le survol mais s'en remet à ses collègues (aucune réponse, décision de Johann).
- betaflight-configurator **#5791** (page cachée) : ouverte, RN: BUGFIX, 3 approbations requises ; approuvée par haslinghuis le 17/11 vers 20 h 22 (notification GitHub du 17/11, capture de Johann ; non revérifiée sur GitHub). nerdCopter : « tab-exit by design ». Réponse de Johann publiée le 16/11 (texte audité) : quitter l'onglet Moteurs arrête déjà les moteurs sur master ; la PR ne réagit qu'à la page cachée.
- betaflight **#16023** (I2C F4, af3a9bc) : Ready for review depuis le 16/11 (vérifié par l'audit, API GitHub, 15:20 UTC). Ne pas cliquer « Update branch » ni « Resolve ». **#16022** (GM3) : non revue le 16/11.

## 3. Défauts encore ouverts

| Gravité | Défaut | Où |
|---|---|---|
| À décider (16/11) | Pilote I2C F4 : TRUEGLIDE a les 2 memsets mais pas la garde TXE de betaflight#16023. Garde ajoutée sur la branche LOCALE `essai/i2c-garde-txe` (2555b914a, +4 −1, non poussée, OUI audit pour le banc seulement). Banc du 16/11 non concluant (section 5) : la garde est justifiée par le code et par les essais Betaflight, sans effet mesuré dans TRUEGLIDE. Seul banc candidat : Mini Pouce + sa config, témoin cb4e3f0 ≥ 600 s | firmware |
| D'origine Betaflight (16/11, CodeRabbit sur #16023) | i2cInit() sort tout de suite si les broches sont déjà prises : la réinitialisation après timeout et son memset ne s'exécutent qu'au démarrage (bus_i2c_stm32f4xx.c l.457 et l.464). Non touché | firmware |
| Choix gardé (audit F9) | Retour batterie : si le GPS Rescue revient après un état « retour impossible », le retour part tout de suite (batterie déjà basse, pilote déjà prévenu ; le manche l'annule) | firmware |
| Dit dans les notes (08/11) | power_guard_current 1 à 9 A sans minimum au firmware (banc) ; relance moteur sans limite de coupures ; retour batterie (F9) ; test moteurs arrêté après 1 s sans renvoi (logiciel du 08/11 requis) ; aucune fonction TrueGlide essayée en vol (TG-P3B) | firmware |
| À surveiller | Relance moteur : vérifiée seulement sans fausse alerte (TG-A1-4), pas sur un vrai moteur calé : garder désactivée par défaut | firmware |
| Corrigé et en ligne (08/11) | /app/ 94634f0 : en 3D (DShot), toucher un seul curseur envoyait 1000 (pleine marche arrière) aux autres moteurs (audit F1) | logiciel, onglet Moteurs |
| Corrigé sur /test/ 1cf71f8, audit en cours | F2 à F8 de l'audit du 07/11 (plage contrôlée avant l'envoi, interrupteur annulable, codes hors ligne refusés par numéro, réglages à risque complétés, onglet ESC une demande à la fois, mineurs) |  logiciel |
| Corrigé sur /test/ 98da639 puis 1cf71f8, audit en cours | CSP (sauf 'unsafe-eval' : Vue compile <app> au démarrage) ; fonds Google après accord ; polices du site hébergées sur le site ; failsafe « Tombe » confirmé ; power_guard_current 1 à 9 A confirmé (0 = arrêt ; minimum non imposé au firmware : 2 A sert au banc) | logiciel, site |
| D'origine Betaflight | 3D changé sans redémarrage | firmware |
| Mineur (prudent) | Interrupteur failsafe (BOXFAILSAFE) actif : « Liaison radio — » alors que la radio est liée | logiciel, Tableau de bord |
| Corrigé sur main 5a2728c (13/11), pas publié | Contrôle quotidien GitHub rouge depuis le 07/11 : msp_motor_guard.c et baro_step_filter.c sans `#include "platform.h"` (make checks). Ligne ajoutée, aucun changement de comportement ; contrôle relancé vert (tests 76/76, .hex OK). Partira avec le prochain firmware, après audit ; cb4e3f0 reste en ligne | firmware |
| Branche d'essai (13/11), pas publiée | Corrections Betaflight 2026.6-maintenance reprises sur correction-bf-2026.6.3 (1362c85) : audit OUI pour firmware d'essai et banc (reprise identique à l'amont), NON pour annoncer une correction du baromètre : le Mini Pouce est en DShot par timer (DEFAULT_DSHOT_BITBANG OFF), la piste betaflight#15533 (bitbang) ne s'y applique pas. Cause des erreurs I2C du Mini Pouce non trouvée. Mesures B1 à B6 de l'audit à faire ; pas de vol avant | firmware |
| À essayer | Onglet ESC : AM32 pas encore essayé sur un vrai ESC AM32 (BLHeli_S / Bluejay essayés ; récupération après flash coupé faite le 10/11) | onglet ESC |

## 4. Journal des audits

| Date | Version | Verdict | Lien |
|---|---|---|---|
| 06/11/2026 | logiciel avant ebde305 | 1er audit : PAS PRÊT, 15 constats | `controle/REPONSE_AUDIT_2026-11-06.md` |
| 06/11/2026 | ebde305 puis 17ca114 | contre-audit (N1 à N4…) | `controle/REPONSE_AUDIT_2026-11-06.md` |
| 06/11/2026 | onglet ESC v16 (29100f6) | vérification en ligne : v16 corrigée (discussion « Audit avant publication – TrueGlide ESC Configurator (PUBLIC) ») | TrueGlide-ESC-Configurator, branche audit/publication-public, docs/audit/VERIFICATION-DEPLOIEMENT-2026-11-06.md |
| 07/11/2026 | ebee781 (sécurité du test moteurs) + firmware ee050eb | OUI pour /app/ (M1 à M4 à faire) | `controle/AUDIT_2026-11-07_moteurs.md` |
| 07/11/2026 | firmwares 4b3d47cba, 33 cartes (sommes, nom de carte, révision contrôlés) | OUI pour firmware/ | gh-pages f0cebe4 |
| 07/11/2026 | 94634f0 (liaison radio « — » quand la carte signale la perte du signal) | OUI pour /app/ (mineur : interrupteur failsafe BOXFAILSAFE affiche aussi « — », sens prudent) | — |
| 07/11/2026 | 7b2dbcb (PID absents du diff all comparés aux valeurs par défaut du firmware) | OUI pour /app/ | gh-pages 0a4c259 |
| 08/11/2026 | 1cf71f8, a991709, fce3fc3 (onglet ESC v18), firmware 5e246cc puis 33 cartes cb4e3f0f2 | OUI pour /app/ (fce3fc3) et pour firmware/ (cb4e3f0f2, sommes, noms de carte et révision contrôlés) ; F9 gardé (choix) | gh-pages acf583d et 674ceca |
| 07/11/2026, 23 h | 94634f0 (recontrôle complet) | NON : F1 bloquant (3D marche arrière), F2 à F8, mineurs ; fiche TG-A1-7 à corriger (batterie pour lire les ESC, essai F1) | audit, message du 07/11 23 h 10 |
| 07/11/2026 | 391767f (molette, dialogues cachés, rxfail/PID, flasheur ; tests.yml exécuté avec succès) | OUI pour /app/ | REPRISE, 07/11 |

## 5. Essais au banc faits (vraie carte : Mini Pouce, SpeedyBee F405 Mini)

| Date | Fiche | Versions | Résultat |
|---|---|---|---|
| 28/10 | TG-P2 | 1791465 | 32 oui sur 32 |
| 29/10 | TG-P2B, TG-P3 | f9fc6d5 | oui (baromètre corrigé ; premiers vols) |
| 30/10 | TG-P3A | avant 24a71eb | A1 à A5 oui |
| 31/10 | TG-P2B, TG-P2C | 24a71eb | baromètre OK |
| 03/11 | TG-V1 (vol intérieur) | 24a71eb | tout oui |
| 07/11 | TG-A1 | 24a71eb + logiciel /test/ | toutes les questions oui (partie 5 refaite ; 6 = onglet ESC v16, 4 ESC BLHeli_S 35 A lus) |
| 07/11 | TG-A1-5 bis (3D + Sauvegarder, batterie branchée) | 24a71eb + ebee781 | aucun mouvement |
| 07/11 | TG-A1-5C (arrêt pendant le test, mode normal, pas 3D) | ee050eb + ebee781 | 3 oui |
| 08/11 | Fiches d'essais du 08/11 (audit) : 0.1, 0.2, TG-A1-7 (7-0, 7A, 7B, 7C-1 à 7C-5), onglet ESC v18 Q0, Q1-1 à 5, Q2-1 à 10, Q3-1 à 4 | cb4e3f0 + /test/ 83011e7 + ESC v18 da0ad00 | tout OUI (12 h 33 à 15 h 17) |
| 08/11 | Mode hors ligne avec la carte remise en Betaflight 2025.12.2 (sauvegarde XPRO du 14/10), vidéos HORS_LIGNE_BETA_TRUEGLIDE | /app/ 83011e7 | OUI : récolte, réglage hors ligne (craft_name = HELLO MINI POUCE), envoi confirmé, relecture « 1 réglage contrôlé, tous en place », nom relu dans Betaflight Configurator |
| 08/11 | Onglet ESC : Q4 (arrêt à « Lire les paramètres », v21) 6/6 ; Q5 (connexion / déconnexion, v22) 5/5 ; journal propre | /test/ 3d44157 + ESC v22 ebaaf7f | OUI |
| 10/11 | Onglet ESC v24 : 2 flashs de l'ESC 4 coupés exprès par Johann à l'étape 3 (17 h 07 : câble USB débranché ; 17 h 10 : batterie débranchée, puis rebranchée et « Lire les paramètres »), ESC lu « FLASH*FAILED », reflash Bluejay 0.19.2 J-H-40 48 kHz réussi les 2 fois (10,9 s), réglages des 4 ESC rechargés depuis la sauvegarde, relecture des 4 ESC OK (journaux esc-configurator-log_2 et _3) | cb4e3f0 + /app/ 7dc6865 (onglet ESC v24 34244fe) | OUI : récupération après un flash coupé faite sur un vrai ESC, sur /app/ |
| 13/11 | TG-B1 : Betaflight App 2026.6.2 contre TRUEGLIDE /app/ 7dc6865 sur Betaflight 2026.6.2 officiel (filmé, écran + téléphone) ; retour cb4e3f0 + sauvegarde du 08/11 (202 réglages identiques) ; ESC lus (4 × J-H-40 Bluejay 0.19 24 kHz, Reversed) ; baro armé avec la radio 33,5 s (debug BARO) | BF 2026.6.2 / cb4e3f0 | Molette : BF monte sans curseur choisi (jusqu'à 1775), TG figé ; page cachée : BF continue (1140, son), TG arrêt net ; baro : 0 erreur I2C, aucun cycle rejeté, baroAlt −0,38 à +0,65 m, saut max 9 cm ; dshot_bitbang OFF, 1 seul appareil I2C (DPS310, pas de compas) |
| 13/11 | Erreur 5 (pilote I2C F4), USB seul sans batterie (confirmé par Johann le 14/11) : firmwares de TEST non publiés, Betaflight 2026.6.2 officiel + abandon de lecture provoqué toutes les 5 s (A) et + correction memset(i2c_ev_state) (B), audit OUI | A sha256 21acbb30…, B 79641781… | A : pression debug BARO[1] plonge sous −10 000 toutes les ~5 s, 0 erreur I2C au compteur en 45 min (bus non bloqué) ; B : pression stable 961 hPa, température 18 °C, 0 erreur. Défaut prouvé dans Betaflight officiel, correction prouvée. Vidéos A et B de Johann. Retour cb4e3f0 + sauvegarde, `version` relu |
| 14/11 | PR Betaflight ouvertes par Johann (contrôle indépendant OUI, textes et commits identiques) : betaflight #16022 GM3 ; configurator #5789 courant read16, #5790 molette (vidéo), #5791 page cachée (2 vidéos) ; betaflight #16023 I2C en BROUILLON (vidéos A/B) | master 44f3c43 / App 9fafdd4 | #16023 : vol avec la cloud build de la PR + journal Blackbox debug BARO avant « ready » (fiche de vol à écrire puis auditer). Workflows App en attente d'approbation (1re contribution). Explications : PR_betaflight_explications.md |
| 14/11 | ESC Mini Pouce : Compensation de démagnétisation Low → High (onglet ESC de /app/, 0.33.0-dev 34244fe), écrite sur les 4 ESC puis relue | 4 × J-H-40 Bluejay 0.19 24 kHz | Relu : High ; timing 30° (High), démarrage 1025/1100, rampe 9x, sens Reversed inchangés. Carte alors sous le firmware de la PR #16023 |
| 14/11 | Vol PR #16023 (firmware built locally from 542c458, debug BARO), Mini Pouce | Betaflight 2026.12.0 (542c45840) | Vol normal ~38 s. 4 lectures baro fausses isolées (0,05 s chacune) : log 2 t=2,033 s P −4551 hPa ; log 3 t=6,502 s P 7931 hPa, t=26,954 s T −132,80 °C, t=33,111 s P 828 hPa. status : I2C 0 errors (le compteur de master ne compte que les blocages). Cause NON établie ; PR reste en brouillon. Journal (UID masqué) et CSV : essais_pr16023/. TRUEGLIDE cb4e3f0, vérifié dans barometer.c : cycle rejeté sur erreur de bus (l.458-477), plage de pression plausible (l.175, 530), filtre des sauts d'altitude (l.444) |
| 15/11 | Synthèse vibration 244 Hz (Mini Pouce), journaux de Johann analysés (orangebox, gyro brut au pic des moteurs). Hors dépôt : journaux non masqués (DeviceUID) | vol PR 542c458 ; tests B et C TRUEGLIDE cb4e3f0 + fichier PID doux / anti-désync | Vibration de rotation, pas un choc : présente tout le vol, suit la vitesse des moteurs (238-256 Hz), retirée par le filtre (1,02 → 0,008). Roulis ÷ tangage : vol PR 5,2 ; test B (nouvelle sangle, l'ancienne touchait la carte côté droit) 3,0 ; test C (hélices 2 ↔ 3 remontées) 2,4, pic 3,23 → 1,66. Même roulis dominant sur châssis 135 mm (KarRmen, BF 2025.12.2) et 122 mm (moteurs 1404, BF 4.4.2) : châssis et moteurs écartés. Tests au banc (TrueGlide à 1300, puis Betaflight à 2000) contradictoires (moteur 1 puis 3) : non fiables. Cause la plus probable, non prouvée : hélices montées avec jeu (trou 2 mm, axe 1,5 mm). À faire : hélices à trou de 1,5 mm, puis un vol. Désyncs : 0 aux tests B et C ; celles de KarRmen (moteur 2, ancien ESC) non reproduites. ESC 2 à 32-33 °C au repos (autres 22-28 °C) : cause non vérifiée |
| 16/11 | Garde TXE dans TRUEGLIDE, carte neuve, USB seul, BANC_reglages_carte_neuve.txt (4 erreurs CLI attendues : osd_uart, vtx_type, vtx_uart, msp_3_uart absents de TRUEGLIDE), flash_erase → save → débrancher/rebrancher, onglet Capteurs. Règle écrite avant avec l'audit : témoin > 0 et garde 0 = la garde marche ; témoin 0 = non concluant ; garde > 0 = arrêt | essai 2555b914a (local, .hex sha256 766ea772…2667) ; témoin cb4e3f0 publié (.hex sha256 ee1d1dfe…2d82) | Garde : 694,4 s + 50,0 s, 0 mauvaise mesure, debug[4] et [5] = 0, status 0 errors (journal sha256 54935dc0…d919, script exécuté par la discussion d'appoint). Témoin : 737,7 s + 4,8 s, 0 mauvaise mesure, debug[4] et [5] = 0, status 0 errors (journal sha256 71cdcc9c…05e7, recompté ici). En-têtes identiques (features 272892936, debug 55, P interval 16, denom 2, bidir 1, même carte). **NON CONCLUANT** (audit). Compilateur contrôlé : cb4e3f0 recompilé = fichier en ligne à 14 octets près (date) |
| — | TG-P3B (premier vol cb4e3f0 + ALT HOLD, Mini Pouce ; fiche auditée OUI 4083ce1) | cb4e3f0 | PAS FAIT |
| — | TG-P3C (fonctions TrueGlide en vol, ancien TG-P3B, à réécrire) | — | PAS FAIT |

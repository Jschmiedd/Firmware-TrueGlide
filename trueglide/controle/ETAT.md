# État de TRUEGLIDE — à lire en premier dans chaque discussion (développement et audit)

Mis à jour le 13/11/2026. Modes d'emploi (hors ligne, mise en route depuis Betaflight, Studio, plan de vol) corrigés le 07/11 et donnés à Johann (hors dépôt). À mettre à jour à chaque changement (version en ligne, règle, défaut, audit, essai).
Détail et historique : `trueglide/REPRISE.md` et `trueglide/controle/RESULTATS.md`.

## 1. En ligne aujourd'hui

| Quoi | Version | Commit | Date |
|---|---|---|---|
| Logiciel /app/ | 7dc6865 (copie exacte de /test/, main-BauRBTPI.js : onglet ESC v24 ; essai de coupure de Johann réussi, sa décision « OUI, IL FAUT TOUT METTRE À JOUR, PUBLIER SUR TEST ET APP », OUI de l'audit) | gh-pages 9fb42b9 ; configurateur main = version/app-2026-11-08 = essai-onglets = 7dc6865 | 10/11/2026 |
| Logiciel /test/ | 25f82b1 (= 7dc6865 + onglet ESC v25 f646695, OUI de l'audit pour /test/ seulement) | gh-pages 797de81 | 11/11/2026 |
| Firmwares en ligne (catalogue de /app/ et /test/) | **cb4e3f0f2** (code = 5e246cc), 33 cartes, modèle TRUEGLIDE, version d'essai (seule SPEEDYBEEF405MINI essayée sur une vraie carte, révisions précédentes ; cb4e3f0 en essai TG-A1-7) ; précédente 4b3d47cba | gh-pages 674ceca (notes, flasheur, accueil) | 08/11/2026 |
| Onglet ESC | /app/ : onglet/v24 (34244fe) ; /test/ : onglet/v25 (f646695 : ESC en FLASH*FAILED sauté à l'écriture), /app/ après essai au banc et « OUI V25 SUR APP » de Johann | TrueGlide-ESC-Configurator | 11/11/2026 |
| Firmware sur le Mini Pouce de Johann | TRUEGLIDE cb4e3f0f2 depuis le 13/11 (flash /app/, `version` relu par Johann ; sauvegarde TRUEGLIDE du 08/11 cb4e3f0 à recoller, vérifiée identique à celle du 31/10 sauf nom et calibration acc) | — | 13/11/2026 |

## 2. Règles permanentes de Johann

- **Format imposé pour TOUS les essais (Johann, 13/11)** : découper en **phases** nommées (Phase A, B, C…) avec un titre qui dit où on va ; dans chaque phase, des étapes **numérotées** (1, 2, 3…), jamais des puces ; une action par ligne, courte ; donner d'abord le plan des phases, puis avancer phase par phase ; image annotée quand c'est utile ; pas de pavés de texte ; dire « avec la radio » ou « dans le logiciel » à chaque fois ; ne pas répondre aux contrôles automatiques par de longs messages.
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
- 11/11 : nouvel auditeur indépendant : discussion session_01VvqUCt5QANL2A4NKbRdzyx (l'ancienne, session_01VvqUCsD9iRLVMzii8FW6NM, ne donne plus d'OK).
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

## 3. Défauts encore ouverts

| Gravité | Défaut | Où |
|---|---|---|
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
| — | TG-P3B (premier vol cb4e3f0 + ALT HOLD, Mini Pouce ; fiche auditée OUI 4083ce1) | cb4e3f0 | PAS FAIT |
| — | TG-P3C (fonctions TrueGlide en vol, ancien TG-P3B, à réécrire) | — | PAS FAIT |

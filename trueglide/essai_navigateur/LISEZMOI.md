# Essai : la carte simulée dans le navigateur (SPEEDYBEE F405 MINI)

Essai de faisabilité du 26 octobre 2026, sur la branche `essai-carte-navigateur`, **sans aucune intégration** dans
TRUEGLIDE CONFIGURATOR ni dans le site. Le but : faire tourner le vrai firmware TRUEGLIDE dans la page, pour régler
une carte débranchée avec le firmware lui-même plutôt qu'avec des réponses enregistrées.

## Ce qui a été fait

1. **Simulateur complet `SITLFULL`** (`src/platform/SIMULATOR/target/SITLFULL/`) : copie du simulateur SITL avec
   les fonctions d'une vraie carte, pour leurs réglages seulement : DShot (et DShot bidirectionnel), filtre dynamique,
   filtre RPM, ralenti dynamique, OSD (positions des éléments, MSP DisplayPort), récepteur série, télémétrie, LED,
   VTX. Aucun moteur, aucune puce vidéo, aucune LED n'est piloté ; aucune vitesse moteur n'est inventée.
2. **Version WebAssembly** (`construire.sh`) : le même firmware, sans réseau ni fils d'exécution. Les ports série
   passent par la mémoire de la page (`tg_entree` pour envoyer, `Module.tgSortie` pour recevoir) ; un
   redémarrage de la carte appelle `Module.tgRedemarrage` et la page relance la carte avec sa mémoire
   (`eeprom.bin`).

Toutes les modifications du code commun sont protégées par `SITLFULL` ou `__EMSCRIPTEN__`.

## Résultats mesurés

| Contrôle | Résultat |
| --- | --- |
| Diff complet de la carte KarRmen (F405 Mini) collé dans le simulateur SITL actuel | 172 / 204 lignes acceptées |
| Même diff dans `SITLFULL` | **204 / 204** |
| Après `save`, redémarrage et `diff all` | toutes les lignes relues à l'identique (*) |
| Version WebAssembly (Node), même essai | **203 / 203** acceptées, toutes gardées après redémarrage (*) |
| Taille | `carte.wasm` 455 Ko + `carte.js` 89 Ko |
| Démarrage dans Chromium | 0,3 à 0,6 s, puis environ 1 s de démarrage du firmware |
| Requête MSP dans Chromium | **24 ms**, stable |
| TRUEGLIDE CONFIGURATOR publié (/test/) relié à la carte simulée dans la page | connexion, onglets Tableau de bord, Ports, Configuration, Moteurs, OSD, Réglages de PID : configuration KarRmen affichée ; nom changé, « Sauvegarder et Redémarrer », reconnexion : nom relu ; aucune erreur de page |
| Firmware de la vraie SPEEDYBEE F405 MINI construit avec et sans l'essai | **identique octet pour octet** (même révision) |

(*) `feature TELEMETRY` n'apparaît pas dans `diff all` parce qu'elle est active d'usine dans le simulateur ; la
commande `feature` la montre bien active. La ligne `serial UART1 …` est retirée de l'essai : dans le simulateur,
l'UART1 sert de console ; dans la version navigateur, la console devra passer par un port à part.

## Écarts connus avant toute intégration

| Écart | État |
| --- | --- |
| Gyroscope à 1 kHz (fréquences fausses dans Configuration) | **Corrigé** : le gyro virtuel tourne à la fréquence du gyro de la vraie carte (ICM42688P : 8 kHz ; boucle PID 4 kHz avec `pid_process_denom = 2`), comme sur la vraie F405 Mini |
| Carte simulée sur le fil de la page | **Corrigé** : elle tourne dans un Web Worker (`carte_worker.js`), sorties regroupées par tour de boucle ; page à 239–243 tours/s (maximum du navigateur) contre 45 avant ; parcours Configurator inchangé, aucune erreur |
| Liste des fonctions écrite à la main | **En cours** (branche `essai-carte-generee`) : sur les 669 réglages de la vraie F405 Mini, il en manque 110 à la carte simulée et elle en a 62 en trop (navigation, boussole) |
| Ports et broches de la vraie carte | **En cours** : la carte simulée n'a ni l'USB (VCP) ni les tables de broches, timers et DMA du F405 |

Mesures en direct : sans mouvement simulé, la carte simulée affiche une charge processeur et une cadence du gyro à 0
(vrai pour la simulation, différent d'une vraie carte) ; à signaler dans le logiciel.

## Reconstruire et rejouer

```
EMSDK=/chemin/emsdk trueglide/essai_navigateur/construire.sh            # obj/navigateur/carte.js + carte.wasm
node trueglide/essai_navigateur/essai_node.cjs diff_de_la_carte.txt     # collage, save, redémarrage, relecture
node trueglide/essai_navigateur/essai_configurateur.cjs                 # Configurator publié + carte simulée dans un Web Worker (Playwright)
```

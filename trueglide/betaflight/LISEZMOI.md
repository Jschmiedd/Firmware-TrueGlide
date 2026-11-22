# Carte simulée Betaflight

Le firmware **Betaflight** exact d'une carte, compilé pour l'ordinateur (essais) ou pour le navigateur (carte hors
ligne du **TRUEGLIDE CONFIGURATOR**), avec la même méthode que la carte simulée TrueGlide
(`trueglide/essai_navigateur/vraie_carte.py`).

- `betaflight-2025.12.2.patch` : changements appliqués à Betaflight 2025.12.2 (carte simulée `simulee/`, script
  `trueglide/vraie_carte.py`, quelques lignes protégées par `TG_CARTE_SIMULEE`). Rien ne change pour la vraie carte.
- `options.txt` : options de construction de chaque vraie carte (construction en ligne de Betaflight).
- `construire.sh DOSSIER CARTE [natif|wasm]` : récupère Betaflight, applique le patch, construit.

Vérifié sur la SPEEDYBEE F405 MINI de KarRmen : après chargement de son `diff all`, le `dump all` de la carte simulée
est identique à celui de la vraie carte (992 réglages sur 992, toutes les autres lignes ; seul le numéro de puce
diffère, remplacé par celui de la vraie carte dans le logiciel).

Betaflight 2026.6.2 (`betaflight-2026.6.2.patch`, ligne `SPEEDYBEEF405MINI_KARRMEN_2026`) : vérifié sur la même
carte le 22/11/2026, carte simulée acceptée (`dump all` identique). Le cloud de Betaflight ajoute `USE_SERIALRX`,
`USE_TELEMETRY` et `USE_TELEMETRY_CRSF` avec CRSF sans les afficher dans « Options » : ils sont dans la ligne.
Construit avec Emscripten 7.0.1 (une version 3.x ne connaît pas `STACK_SIZE`) ; avec arm-none-eabi-gcc 13.2,
ajouter `GCC_REQUIRED_VERSION=13.2.1` (Betaflight 2026.6.2 attend 13.3.1).

Cible `betaflight` (22/11/2026) : la même carte pour l'application Betaflight, avec des noms anglais
(`simulated-board.js`, `simulated-board.wasm`, `simulated-board-worker.js`, `SimulatedBoard`, `sim_input`,
`sim_set_uid`, messages `start`/`input`/`read`/`ready`/`output`/`restart`/`memory`). La cible `wasm` (noms
TrueGlide) ne change pas. Même firmware : `dump all` identique à la cible `wasm` (2025.12.2 et 2026.6.2).
Messages du journal en anglais avec cette cible (`SIM_TEXT`, `simulee/carte_simulee.h`).

Ensuite : `node scripts/sync-trueglide-simulee.mjs <DOSSIER>/obj/vraie_carte/<CARTE>/wasm` dans le Configurator.
Licence : GPL-3.0, comme Betaflight.

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

Ensuite : `node scripts/sync-trueglide-simulee.mjs <DOSSIER>/obj/vraie_carte/<CARTE>/wasm` dans le Configurator.
Licence : GPL-3.0, comme Betaflight.

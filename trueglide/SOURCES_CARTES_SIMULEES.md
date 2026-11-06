# Sources des cartes simulées (carte.wasm) de TrueGlide Configurator

TrueGlide Configurator contient des cartes simulées (`simulee/<CARTE>/<FIRMWARE>/carte.wasm`) : le firmware exact d'une
carte, compilé pour le navigateur. Licence GPL-3.0, comme les firmwares. Leurs sources complètes (code, scripts de
construction, correctif appliqué à Betaflight) sont publiques dans ce dépôt, branche `essai-vraie-carte` :

| Carte simulée | Sources | Construction |
|---|---|---|
| SPEEDYBEEF405MINI / BTFL_2026.6.2 (firmware TrueGlide) | branche `essai-vraie-carte`, commit `35b1d53` : `src/platform/STM32/simulee/`, `trueglide/essai_navigateur/` | `python3 trueglide/essai_navigateur/vraie_carte.py SPEEDYBEEF405MINI wasm` (Emscripten) |
| SPEEDYBEEF405MINI / BTFL_2025.12.2 (Betaflight 2025.12.2) | même commit : `trueglide/betaflight/betaflight-2025.12.2.patch`, `options.txt`, `construire.sh` | `trueglide/betaflight/construire.sh` (appliqué à Betaflight 2025.12.2) |

Les deux carte.wasm actuelles ont été ajoutées au logiciel le 26/10/2026 à 21:26 (commit 2dbcb07 du configurateur) ;
35b1d53 (26/10/2026 21:01) est le dernier commit de la branche avant cet ajout.

Lien : https://github.com/Jschmiedd/Firmware-TrueGlide/tree/35b1d53

Ajouté après l'audit indépendant du 06/11/2026 (constat 15) : les sources étaient publiées, mais seulement sur cette
branche, sans lien depuis `main` ni depuis le logiciel.

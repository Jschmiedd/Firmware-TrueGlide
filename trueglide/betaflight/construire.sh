#!/bin/bash
# TrueGlide : carte simulée Betaflight (firmware Betaflight exact d'une carte, pour la carte hors ligne du
# TrueGlide Configurator). Récupère Betaflight et sa configuration de carte, applique betaflight-<VERSION>.patch
# (carte simulée : src/platform/STM32/simulee/ et trueglide/vraie_carte.py), puis construit.
# Usage : trueglide/betaflight/construire.sh DOSSIER CARTE [natif|wasm]
#   DOSSIER : où récupérer Betaflight (créé s'il n'existe pas) ; CARTE : ligne de options.txt
#   (arm-none-eabi-gcc dans le PATH pour lire la vraie construction ; emcc pour wasm)
set -euo pipefail
ICI=$(cd "$(dirname "$0")" && pwd)
DOSSIER=$1; CARTE=$2; CIBLE=${3:-natif}
ligne=$(grep -E "^$CARTE " "$ICI/options.txt") || { echo "carte inconnue dans options.txt : $CARTE"; exit 1; }
read -r _ CONFIG VERSION CONFIG_REV OPTIONS <<<"$ligne"
if [ ! -d "$DOSSIER/.git" ]; then
    git clone -q --depth 1 --branch "$VERSION" https://github.com/betaflight/betaflight.git "$DOSSIER"
    git -C "$DOSSIER" apply "$ICI/betaflight-$VERSION.patch"
    git clone -q https://github.com/betaflight/config.git "$DOSSIER/src/config"
    git -C "$DOSSIER/src/config" checkout -q "$CONFIG_REV"
fi
cd "$DOSSIER" && TG_OPTIONS="$OPTIONS" python3 trueglide/vraie_carte.py "$CONFIG" "$CIBLE"

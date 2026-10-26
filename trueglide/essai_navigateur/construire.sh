#!/bin/bash
# Essai : construit la carte simulée SITLFULL en WebAssembly (carte.js + carte.wasm), sans réseau ni fils d'exécution.
# Prérequis : emsdk installé et activé (EMSDK=/chemin/emsdk). Sortie : dossier donné en argument (par défaut obj/navigateur).
set -e
cd "$(dirname "$0")/../.."
SORTIE="${1:-obj/navigateur}"
source "${EMSDK:?indiquer EMSDK=/chemin/emsdk}/emsdk_env.sh" >/dev/null 2>&1
mkdir -p "$SORTIE"
# compilation : seule l'édition de liens de make échoue (pas de script de liens en WebAssembly), elle est refaite ensuite
make MODELE=TRUEGLIDE TARGET=SITLFULL OBJECT_DIR=obj/wasm CROSS_CC=emcc CROSS_CXX=em++ SIZE=true OBJCOPY=true \
    "CFLAGS_DISABLED=-flto=auto -fuse-linker-plugin -Werror -Wunsafe-loop-optimizations -Ofast" \
    LTO_FLAGS="-O2" LD_FLAGS="-O2" -j8 -k >"$SORTIE/make.log" 2>&1 || true
emcc -O2 $(find obj/wasm/SITLFULL -name "*.o") -lm -Wl,--allow-multiple-definition \
    -sSTACK_SIZE=1048576 -sALLOW_MEMORY_GROWTH -sMODULARIZE -sEXPORT_NAME=TrueGlideCarte \
    -sEXPORTED_FUNCTIONS=_main,_tg_entree,_malloc,_free -sEXPORTED_RUNTIME_METHODS=FS,HEAPU8 \
    -sENVIRONMENT=web,worker,node -sEXIT_RUNTIME=0 -o "$SORTIE/carte.js"
ls -la "$SORTIE/carte.js" "$SORTIE/carte.wasm"
cp "$(dirname "$0")/carte_worker.js" "$SORTIE/carte_worker.js"

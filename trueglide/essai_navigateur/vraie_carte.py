#!/usr/bin/env python3
# TrueGlide : construit la carte simulée « vraie carte » : le firmware exact d'une carte STM32F4 (mêmes fichiers, même
# config.h, mêmes tables de broches, timers et DMA), compilé pour l'ordinateur (natif, pour les essais) ou pour le
# navigateur (WebAssembly). Seuls les fichiers qui touchent directement au matériel sont remplacés par ceux de
# src/platform/STM32/simulee/ (démarrage, horloge, mémoire des réglages, USB, interruptions).
#
# Usage : python3 trueglide/essai_navigateur/vraie_carte.py [CARTE] [natif|wasm]
#         (arm-none-eabi-gcc dans le PATH pour lire la vraie construction ; emcc pour wasm)

import concurrent.futures
import fnmatch
import os
import re
import shlex
import subprocess
import sys

RACINE = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# fichiers de la vraie carte remplacés par la carte simulée (motifs sur le chemin trouvé)
REMPLACES = [
    '*.s', '*.S',                                   # démarrage en assembleur
    '*/STM32/startup/system_stm32f4xx.c',           # horloges du microcontrôleur
    '*/STM32/system_stm32f4xx.c',
    '*/common/stm32/system.c',                      # temps, délais, redémarrage
    '*/common/stm32/config_flash.c',                # écriture des réglages en mémoire flash
    '*/common/stm32/fault_handlers.c',
    '*/STM32/persistent.c',
    '*/STM32/serial_usb_vcp.c',                     # port USB : relié à la page par la mémoire
    '*/STM32/vcpf4/*',
    '*STM32_USB_Device_Library/*',
    '*STM32_USB_OTG_Driver/*',
    '*/STM32/usb_msc_f4xx.c',                       # clé USB (stockage de masse) : pas dans la simulation
    '*/STM32/usbd_msc_desc.c',
    '*/main/msc/usbd_storage*',
    '*/common/stm32/msc_sdio_storage.c',
    '*/drivers/usb_msc_common.c',
    '*/io/usb_msc.c',
    '*/io/usb_cdc_hid.c',
]

# fichiers gardés dont quelques fonctions sont remplacées (renommées pour ce fichier seulement)
RENOMMES = {
    'src/platform/STM32/bus_spi_stdperiph.c': ['-DspiSequenceStart=tgMaterielSpiSequenceStart',
                                               '-DspiInternalReadWriteBufPolled=tgMaterielSpiReadWriteBufPolled'],
    'src/platform/STM32/bus_i2c_stm32f4xx.c': ['-Di2cWriteBuffer=tgMaterielI2cWriteBuffer', '-Di2cWrite=tgMaterielI2cWrite',
                                               '-Di2cReadBuffer=tgMaterielI2cReadBuffer', '-Di2cRead=tgMaterielI2cRead',
                                               '-Di2cBusy=tgMaterielI2cBusy',
                                               '-Di2cGetErrorCounter=tgMaterielI2cGetErrorCounter'],
    'lib/main/STM32F4/Drivers/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c': [
        '-DADC_SoftwareStartInjectedConv=tgMaterielAdcStartInjected'],
}

ARM = re.compile(r'^(-mthumb|-mcpu=.*|-march=.*|-mfloat-abi=.*|-mfpu=.*|-flto.*|-fuse-linker-plugin|-Werror|'
                 r'-Wunsafe-loop-optimizations|-specs=.*|-nostdlib|-O.*|-g.*|-MMD|-MP|-MF.*)$')


def make(variable, carte):
    r = subprocess.run(['make', '--no-print-directory', f'--eval=lire: ; @echo $({variable})', 'lire',
                        'MODELE=TRUEGLIDE', f'CONFIG={carte}', 'OBJECT_DIR=obj/vraie_carte/lecture'],
                       cwd=RACINE, capture_output=True, text=True, check=True)
    return r.stdout.strip().splitlines()[-1]


def trouver(f, dossiers):
    for d in dossiers:
        p = os.path.normpath(os.path.join(RACINE, d, f))
        if os.path.isfile(p):
            return os.path.relpath(p, RACINE)
    raise FileNotFoundError(f)


def main():
    carte = sys.argv[1] if len(sys.argv) > 1 else 'SPEEDYBEEF405MINI'
    cible = sys.argv[2] if len(sys.argv) > 2 else 'natif'
    # make écrit les valeurs entre guillemets (-D__REVISION__="…") : découpage simple, guillemets gardés
    drapeaux = [d for d in make('CFLAGS', carte).split() if not ARM.match(d)]
    dossiers = ['.'] + make('VPATH', carte).split(':')
    sources = []
    for f in dict.fromkeys(make('SRC', carte).split()):
        p = trouver(f, dossiers)
        if not any(fnmatch.fnmatch('./' + p, m) for m in REMPLACES):
            sources.append(p)
    simulee = 'src/platform/STM32/simulee'
    sources += sorted(os.path.join(simulee, f) for f in os.listdir(os.path.join(RACINE, simulee)) if f.endswith('.c'))

    sortie = os.path.join(RACINE, 'obj/vraie_carte', carte, cible)
    os.makedirs(sortie, exist_ok=True)
    if cible == 'wasm':
        cc = ['emcc', '-O2']
    else:
        cc = ['gcc', '-O1', '-g', '-fno-pie']
    commun = drapeaux + ['-DTG_CARTE_SIMULEE', '-include', os.path.join(simulee, 'cmsis_hote.h'), '-Wno-error']

    def compiler(src):
        obj = os.path.join(sortie, src.replace('/', '__') + '.o')
        r = subprocess.run(cc + commun + RENOMMES.get(src, []) + ['-c', src, '-o', obj], cwd=RACINE, capture_output=True, text=True)
        return src, obj, r.returncode, r.stderr

    objets, erreurs = [], []
    with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as ex:
        for src, obj, code, err in ex.map(compiler, sources):
            if code:
                erreurs.append((src, err))
            else:
                objets.append(obj)
    print(f'{carte} ({cible}) : {len(sources)} fichiers, {len(erreurs)} en erreur')
    for src, err in erreurs:
        premiere = ' | '.join(l.strip() for l in err.splitlines() if 'rror' in l)[:400] or err.strip()[:200]
        print(f'  {src} : {premiere}')
    if erreurs:
        sys.exit(1)

    if cible == 'wasm':
        lien = ['emcc', '-O2'] + objets + ['-lm', '-Wl,--allow-multiple-definition', '-sSTACK_SIZE=1048576', '-sALLOW_MEMORY_GROWTH', '-sMODULARIZE',
                                           '-sEXPORT_NAME=TrueGlideCarte',
                                           '-sEXPORTED_FUNCTIONS=_main,_tg_entree,_tg_definir_uid,_malloc,_free',
                                           '-sEXPORTED_RUNTIME_METHODS=FS,HEAPU8', '-sENVIRONMENT=web,worker,node',
                                           '-sEXIT_RUNTIME=0', '-o', os.path.join(sortie, 'carte.js')]
    else:
        lien = ['gcc', '-no-pie', '-Wl,--gc-sections'] + objets + ['-lm', '-Wl,-T,src/platform/SIMULATOR/link/sitl.ld',
                                              '-o', os.path.join(sortie, 'carte')]
    r = subprocess.run(lien, cwd=RACINE, capture_output=True, text=True)
    if r.returncode:
        manquants = sorted(set(re.findall(r"undefined reference to `([^']+)'|undefined symbol: (\S+)", r.stderr)))
        print('édition de liens :', len(manquants), 'symboles manquants')
        for m in manquants:
            print('  ', ''.join(m))
        print('\n'.join(l for l in r.stderr.splitlines() if 'multiple definition' in l or 'error' in l.lower())[:3000])
        sys.exit(1)
    print('construit :', os.path.relpath(lien[-1], RACINE))


if __name__ == '__main__':
    main()

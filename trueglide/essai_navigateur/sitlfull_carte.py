#!/usr/bin/env python3
# TrueGlide : génère la liste des fonctions de la carte simulée SITLFULL depuis la vraie cible d'une carte.
#
# La carte simulée doit avoir exactement les fonctions (et donc les réglages) de la vraie carte : le préprocesseur
# ARM donne toutes les macros USE_* de la vraie construction (config.h de la carte + common_pre/post), puis on
# compare avec la construction SITLFULL. Le fichier écrit (carte_generee.h, inclus à la fin de common_post.h pour
# SITLFULL seulement) ajoute ce qui manque et retire ce que la vraie carte n'a pas.
#
# Les fonctions purement matérielles du microcontrôleur (DMA, timers, USB OTG…) ne peuvent pas exister dans le
# simulateur : elles sont listées dans MATERIEL et ne changent aucun réglage (contrôlé par trueglide/essai_navigateur/sitlfull_comparer.py).
#
# Usage : python3 trueglide/essai_navigateur/sitlfull_carte.py SPEEDYBEEF405MINI    (arm-none-eabi-gcc et gcc dans le PATH)

import os
import re
import shlex
import subprocess
import sys

RACINE = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SORTIE = os.path.join(RACINE, 'src/platform/SIMULATOR/target/SITLFULL/carte_generee.h')

# fonctions du microcontrôleur lui-même, sans réglage : jamais dans le simulateur
MATERIEL = {
    'USE_STDPERIPH_DRIVER', 'USE_DMA_SPEC', 'USE_TIMER_AF', 'USE_TIMER_DMA', 'USE_TIMER_MGMT', 'USE_TX_IRQ_HANDLER',
    'USE_ULPI_PHY', 'USE_USB_OTG_FS', 'USE_FAST_DATA', 'USE_PERSISTENT_OBJECTS', 'USE_SPI_DMA_ENABLE_EARLY',
}

# fonctions propres au simulateur, gardées : capteurs virtuels (la carte simulée n'a pas de vrais capteurs),
# arguments de lancement
SIMULATEUR = {'USE_VIRTUAL_GYRO', 'USE_VIRTUAL_ACC', 'USE_VIRTUAL_BARO', 'USE_MAIN_ARGS'}


def drapeaux(*args):
    r = subprocess.run(['make', '--no-print-directory', '--eval=drapeaux: ; @echo $(CFLAGS)', 'drapeaux',
                        'MODELE=TRUEGLIDE'] + list(args), cwd=RACINE, capture_output=True, text=True, check=True)
    return shlex.split(r.stdout.strip().splitlines()[-1])


# valeurs du microcontrôleur de la vraie carte reprises telles quelles (ports d'entrées-sorties, identifiant, réglages
# d'usine qui dépendent du microcontrôleur)
VALEURS = re.compile(r'^(TARGET_IO_PORT[A-K]|TARGET_BOARD_IDENTIFIER|DEFAULT_CPU_OVERCLOCK)$')


def toutes(cc, flags):
    r = subprocess.run([cc] + flags + ['-dM', '-E', 'src/main/fc/init.c'], cwd=RACINE, capture_output=True,
                       text=True, check=True)
    return dict(re.findall(r'^#define (\w+)(?:\(.*?\))?[ ]?(.*)$', r.stdout, re.M))


def macros(cc, flags):
    return {m for m in toutes(cc, flags) if m.startswith('USE_')}


def ecrire(carte, ajouter, retirer, valeurs=None):
    lignes = [
        f'// GÉNÉRÉ par trueglide/essai_navigateur/sitlfull_carte.py {carte} : ne pas modifier à la main.',
        f'// Fonctions de la vraie carte {carte} pour la carte simulée SITLFULL (inclus à la fin de common_post.h).',
        '#pragma once',
        '',
        f'#define SITLFULL_CARTE "{carte}"',
        '',
        '// fonctions de la vraie carte',
    ]
    lignes += [f'#ifndef {m}\n#define {m}\n#endif' for m in sorted(ajouter)]
    lignes += ['', '// valeurs du microcontrôleur de la vraie carte']
    for m, v in sorted((valeurs or {}).items()):
        lignes += [f'#undef {m}', f'#define {m} {v}']
    lignes += ['', '// fonctions que la vraie carte n\'a pas']
    lignes += [f'#undef {m}' for m in sorted(retirer)]
    open(SORTIE, 'w').write('\n'.join(lignes) + '\n')


def main():
    carte = sys.argv[1] if len(sys.argv) > 1 else 'SPEEDYBEEF405MINI'
    tout = toutes('arm-none-eabi-gcc', drapeaux(f'CONFIG={carte}', 'OBJECT_DIR=obj/sitlfull_gen'))
    vraie = {m for m in tout if m.startswith('USE_')} - MATERIEL
    valeurs = {m: v for m, v in tout.items() if VALEURS.match(m)}
    # simulateur sans la liste générée (fichier vide), pour savoir quoi retirer
    ecrire(carte, set(), set())
    simu = macros('gcc', drapeaux('TARGET=SITLFULL', f'SITLFULL_CARTE={carte}', 'OBJECT_DIR=obj/sitlfull_gen'))
    ecrire(carte, vraie, simu - vraie - SIMULATEUR, valeurs)
    print(f'{carte} : {len(vraie)} fonctions ; {len(vraie - simu)} ajoutées, {len(simu - vraie - SIMULATEUR)} retirées')


if __name__ == '__main__':
    main()

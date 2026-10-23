#!/usr/bin/env python3
"""Génère js/cartes.js : pour chaque carte de src/config/configs (après « make configs ») : fabricant, processeur,
gyroscopes possibles, nombre de sorties moteur, DShot bidirectionnel activé par défaut, capteur de courant, ports série
(UART et LPUART ayant une broche, fonctions par défaut), récepteur intégré (SPI), GPS présent dans TRUEGLIDE.
Usage, depuis la racine du dépôt : python3 trueglide/studio/tools/extract_cartes.py"""
import json, re
from pathlib import Path

RACINE = Path(__file__).resolve().parents[3]
# Processeurs dont le firmware TRUEGLIDE (MODELE=STANDARD) contient le GPS : mesuré sur les 621 firmwares compilés le
# 22/10/2026 (révision 1791465a6), présence du réglage gps_provider. Le GPS y suit la taille de la mémoire (1 Mo et plus).
# Ailleurs (F411, F722, G474, H750…) : seulement TRUENAV et TRUENAV_LIGHT.
GPS_STANDARD = {'AT32F435G', 'AT32F435M', 'STM32F405', 'STM32F745', 'STM32H563', 'STM32H725', 'STM32H730', 'STM32H735',
                'STM32H743', 'STM32N657'}
SANS_GPS_STANDARD = {'STM32C562', 'STM32F411', 'STM32F446', 'STM32F722', 'STM32G474', 'STM32H750'}
# Fonction donnée par défaut à un port (src/main/io/serial.c, pgResetFn_serialConfig), dans l'ordre du firmware :
# la dernière affectation d'un port l'emporte.
DEFAUTS = [('MSP_UART', 1), ('GPS_UART', 2), ('SERIALRX_UART', 64), ('SBUS_TELEMETRY_UART', 32), ('ESC_SENSOR_UART', 1024),
           ('VTX_SMARTAUDIO_UART', 2048), ('VTX_TRAMP_UART', 8192), ('VTX_MSP_UART', 131072),
           ('MSP_DISPLAYPORT_UART', 131073), ('USE_MSP_UART', 1)]


def nom_port(v):
    """SERIAL_PORT_USART2 -> UART2 (nom de la commande serial du CLI)."""
    m = re.match(r'SERIAL_PORT_(?:US|U|LPU)ART(\d+)$', v.split()[0] if v else '')
    if not m:
        return None
    return ('LPUART' if 'LPUART' in v else 'UART') + m.group(1)


cartes = {}
for cfg in sorted((RACINE / 'src' / 'config' / 'configs').glob('*/*/config.h')):
    t = cfg.read_text(errors='replace')
    d = dict(re.findall(r'^#define[ \t]+(\w+)[ \t]*(.*?)[ \t]*$', t, re.M))
    nom = d.get('BOARD_NAME', cfg.parent.name)
    gyros = sorted({re.sub(r'^USE_(ACCGYRO|GYRO_SPI|GYRO)_', '', k) for k in d if re.match(r'USE_(ACCGYRO|GYRO_SPI)_', k)})
    cartes[nom] = {
        'f': d.get('MANUFACTURER_ID', cfg.parent.parent.name),
        'mcu': d.get('FC_TARGET_MCU', '?'),
        'g': gyros,
        'm': len([k for k in d if re.match(r'MOTOR\d+_PIN$', k)]),
        'bidir': 'DEFAULT_DSHOT_TELEMETRY' in d and 'ON' in d['DEFAULT_DSHOT_TELEMETRY'],
        'courant': d.get('ADC_CURR_PIN', d.get('ADC_CURRENT_PIN', 'NONE')).split()[0] != 'NONE',
    }
    mcu = cartes[nom]['mcu']
    if mcu in GPS_STANDARD or mcu in SANS_GPS_STANDARD:
        cartes[nom]['gps'] = mcu in GPS_STANDARD
    # ports série : "TR" (TX et RX), "T" ou "R" selon les broches déclarées
    ports = {}
    for k, v in d.items():
        m = re.match(r'((?:LP)?UART)(\d+)_(TX|RX)_PIN$', k)
        if m and v.split()[0] != 'NONE':
            ports.setdefault(m.group(1) + m.group(2), set()).add(m.group(3)[0])
    if ports:
        ordre = sorted(ports, key=lambda n: (n.startswith('LP'), int(re.sub(r'\D', '', n))))
        cartes[nom]['p'] = {n: ''.join(sorted(ports[n], reverse=True)) for n in ordre}
    defaut = {}
    for macro, fonction in DEFAUTS:
        if macro == 'GPS_UART' and not cartes[nom].get('gps'):
            continue
        n = nom_port(d.get(macro, ''))
        if n:
            defaut[n] = fonction
    if defaut:
        cartes[nom]['d'] = defaut
    # récepteur série par défaut : CRSF (src/main/pg/rx.c) sauf si la carte en déclare un autre
    rx = d.get('SERIALRX_PROVIDER', '').split()
    if rx and rx[0].replace('SERIALRX_', '') != 'CRSF':
        cartes[nom]['rx'] = rx[0].replace('SERIALRX_', '')
    proto = d.get('RX_SPI_DEFAULT_PROTOCOL', '').split()
    if proto:
        cartes[nom]['rxspi'] = proto[0].replace('RX_SPI_', '')
sortie = RACINE / 'trueglide' / 'studio' / 'js' / 'cartes.js'
sortie.write_text('// Généré par tools/extract_cartes.py depuis src/config/configs : ne pas modifier à la main.\n'
                  '// TrueGlide - Copyright (C) 2026 TalkTrueFPV. Derived from Betaflight.\n// Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.\n'
                  '(function (root) {\n  var CARTES = ' + json.dumps(cartes, ensure_ascii=False, sort_keys=True, separators=(',', ':')) + ';\n'
                  '  if (typeof module !== "undefined") module.exports = CARTES; else root.TF_CARTES = CARTES;\n'
                  '})(this);\n')
print(len(cartes), 'cartes ->', sortie)

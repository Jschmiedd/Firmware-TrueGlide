"""TG-A1-5 (2e relecture, point 1 et point 4) : sécurité du test moteurs par MSP, vérifiée sur le firmware compilé
pour PC (make TARGET=SITL MODELE=TRUEGLIDE). On lit les valeurs réellement envoyées aux moteurs (MSP_MOTOR).

Cas vérifiés :
  A. le port qui a lancé le test interroge la carte toutes les 50 ms : la valeur de test reste appliquée (pas d'arrêt
     intempestif pendant un vrai test) ;
  B. ce port se tait, un AUTRE port MSP (comme une vidéo HD Walksnail en DisplayPort) continue à parler toutes les
     20 ms : les moteurs reviennent quand même à l'arrêt après ~1 s ;
  C. enregistrement EEPROM (MSP_EEPROM_WRITE) : moteurs remis à l'arrêt tout de suite ;
  D. 3D activé puis enregistré (sans redémarrage) : moteurs remis à l'arrêt DU PILOTE QUI TOURNE (en PWM la valeur du
     démarrage, celle de l'ESC jusqu'au redémarrage ; 3e relecture du 07/11 : jamais le neutre 3D envoyé à un ESC
     calibré sans 3D) ; une valeur de test d'avant ne reste pas ;
  E. 3D coupé puis enregistré : idem.

Lancer : python3 trueglide/sitl_bench/test_msp_motor_safety.py   (code de sortie 0 = tout bon)
"""
import os
import shutil
import socket
import struct
import subprocess
import sys
import tempfile
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
ELF = os.path.join(ROOT, 'obj', 'main', 'trueglide_SITL.elf')

MSP_FEATURE_CONFIG = 36
MSP_SET_FEATURE_CONFIG = 37
MSP_REBOOT = 68
MSP_STATUS = 101
MSP_MOTOR = 104
MSP_SET_MOTOR = 214
MSP_EEPROM_WRITE = 250
MSP_CF_SERIAL_CONFIG = 54
MSP_SET_CF_SERIAL_CONFIG = 55
FEATURE_3D = 1 << 12


class Port:
    def __init__(self, tcp):
        self.tcp = tcp
        self.s = None
        for _ in range(100):
            try:
                self.s = socket.create_connection(('127.0.0.1', tcp), timeout=2)
                break
            except OSError:
                time.sleep(0.1)
        if self.s is None:
            raise RuntimeError('port TCP %d injoignable' % tcp)
        self.buf = b''

    def msp(self, cmd, payload=b''):
        n = len(payload)
        c = n ^ cmd
        for b in payload:
            c ^= b
        self.s.sendall(b'$M<' + bytes([n, cmd]) + payload + bytes([c]))
        t0 = time.time()
        while time.time() - t0 < 2:
            i = self.buf.find(b'$M')
            if i >= 0 and len(self.buf) >= i + 5:
                ln = self.buf[i + 3]
                if len(self.buf) >= i + 6 + ln:
                    rcmd = self.buf[i + 4]
                    data = self.buf[i + 5:i + 5 + ln]
                    self.buf = self.buf[i + 6 + ln:]
                    if rcmd == cmd:
                        return data
                    continue
            try:
                self.s.settimeout(0.5)
                self.buf += self.s.recv(4096)
            except socket.timeout:
                pass
        raise TimeoutError('pas de réponse MSP %d sur %d' % (cmd, self.tcp))

    def close(self):
        try:
            self.s.close()
        except OSError:
            pass


class Sitl:
    def __init__(self, workdir):
        self.dir = workdir
        self.p = None

    def start(self):
        self.p = subprocess.Popen([ELF], cwd=self.dir, stdout=open(os.path.join(self.dir, 'sitl.log'), 'a'),
                                  stderr=subprocess.STDOUT)
        time.sleep(0.8)

    def stop(self):
        if self.p:
            self.p.kill()
            self.p.wait()
            self.p = None
            time.sleep(0.5)


def motors(port):
    d = port.msp(MSP_MOTOR)
    return list(struct.unpack('<8H', d[:16]))[:4]


def set_motors(port, v):
    port.msp(MSP_SET_MOTOR, struct.pack('<8H', *([v] * 4 + [0] * 4)))


def features(port):
    return struct.unpack('<I', port.msp(MSP_FEATURE_CONFIG)[:4])[0]


def set_features(port, mask):
    port.msp(MSP_SET_FEATURE_CONFIG, struct.pack('<I', mask))


results = []


def check(name, ok, detail):
    results.append(ok)
    print(('OK   ' if ok else 'ÉCHEC') + ' ' + name + ' : ' + detail)


def enable_msp_on_uart2(sitl):
    """UART2 (TCP 5762) en MSP, comme un second appareil MSP sur la carte."""
    p = Port(5761)
    d = p.msp(MSP_CF_SERIAL_CONFIG)
    size = 7
    out = b''
    for i in range(len(d) // size):
        ident, mask, msp_b, gps_b, tel_b, per_b = struct.unpack('<BHBBBB', d[i * size:(i + 1) * size])
        if ident == 52:  # UART2 (SERIAL_PORT_UART2 = 51 + 1)
            mask |= 1  # FUNCTION_MSP
        out += struct.pack('<BHBBBB', ident, mask, msp_b, gps_b, tel_b, per_b)
    p.msp(MSP_SET_CF_SERIAL_CONFIG, out)
    p.msp(MSP_EEPROM_WRITE)
    p.close()
    sitl.stop()
    sitl.start()


def main():
    if not os.path.exists(ELF):
        print('Construire d\'abord : make TARGET=SITL MODELE=TRUEGLIDE')
        return 2
    work = tempfile.mkdtemp(prefix='tg_msp_motor_')
    sitl = Sitl(work)
    sitl.start()
    try:
        enable_msp_on_uart2(sitl)
        usb = Port(5761)
        other = Port(5762)
        try:
            other.msp(MSP_STATUS)
        except TimeoutError:
            check('second port MSP (UART2)', False, 'UART2 ne répond pas en MSP')
            return 1

        f = features(usb)
        if f & FEATURE_3D:
            set_features(usb, f & ~FEATURE_3D)
            usb.msp(MSP_EEPROM_WRITE)
            time.sleep(0.3)
        rest = motors(usb)
        check('arrêt au départ (3D coupé)', all(v == rest[0] for v in rest), 'moteurs %s' % rest)
        stop = rest[0]

        # A. test actif : le logiciel renvoie les valeurs toutes les 300 ms (et interroge toutes les 50 ms) pendant 3 s
        set_motors(usb, 1100)
        seen = []
        t0 = time.time()
        last_set = t0
        while time.time() - t0 < 3.0:
            if time.time() - last_set >= 0.3:
                set_motors(usb, 1100)
                last_set = time.time()
            seen.append(motors(usb))
            time.sleep(0.05)
        check('A test actif, valeurs renvoyées toutes les 300 ms', all(m == [1100] * 4 for m in seen),
              '%d relevés, tous 1100 : %s' % (len(seen), all(m == [1100] * 4 for m in seen)))

        # A bis (audit du 07/11/2026, F4) : le port du test ne fait plus qu'interroger (onglet caché, logiciel bloqué) :
        # la garde n'est plus entretenue, arrêt seul en ~1 s
        t0 = time.time()
        back = None
        last = None
        while time.time() - t0 < 4.0:
            last = motors(usb)
            if last == [stop] * 4:
                back = time.time() - t0
                break
            time.sleep(0.05)
        check('A bis interrogation seule du port du test : arrêt en ~1 s', back is not None and 0.7 <= back <= 1.6,
              'arrêt après %s s, dernier relevé %s' % (None if back is None else round(back, 2), last))
        set_motors(usb, 1100)

        # B. le port du test se tait, l'autre port parle toutes les 20 ms
        t0 = time.time()
        back = None
        last = None
        while time.time() - t0 < 4.0:
            last = motors(other)
            if last == [stop] * 4:
                back = time.time() - t0
                break
            time.sleep(0.02)
        check('B autre port MSP seul (Walksnail)', back is not None and 0.8 <= back <= 1.6,
              'arrêt %s après %s s (attendu ~1 s), dernier relevé %s' % (stop, None if back is None else round(back, 2), last))

        # B bis. et ça reste à l'arrêt
        time.sleep(0.5)
        check('B bis reste à l\'arrêt', motors(usb) == [stop] * 4, 'moteurs %s' % motors(usb))

        # C. enregistrement EEPROM pendant un test
        set_motors(usb, 1100)
        before = motors(usb)
        usb.msp(MSP_EEPROM_WRITE)
        time.sleep(0.1)
        after = motors(usb)
        check('C enregistrement remet à l\'arrêt', before == [1100] * 4 and after == [stop] * 4,
              'avant %s, après %s' % (before, after))

        # D. 3D activé et enregistré alors qu'une valeur de test était appliquée
        set_motors(usb, 1100)
        set_features(usb, features(usb) | FEATURE_3D)
        usb.msp(MSP_EEPROM_WRITE)
        time.sleep(0.2)
        m3d = motors(usb)
        check('D 3D enregistré : arrêt du pilote qui tourne', m3d == [stop] * 4, 'moteurs %s (arrêt au démarrage %s)' % (m3d, stop))

        # E. 3D coupé et enregistré alors qu'une valeur de test était appliquée
        set_motors(usb, 1100)
        set_features(usb, features(usb) & ~FEATURE_3D)
        usb.msp(MSP_EEPROM_WRITE)
        time.sleep(0.2)
        m = motors(usb)
        check('E 3D coupé enregistré : arrêt du pilote qui tourne', m == [stop] * 4, 'moteurs %s' % m)
        usb.close()
        other.close()
    finally:
        sitl.stop()
        shutil.rmtree(work, ignore_errors=True)
    print('%d / %d' % (sum(results), len(results)))
    return 0 if all(results) else 1


if __name__ == '__main__':
    sys.exit(main())

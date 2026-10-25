"""Tests du centrage (protocole centrage_v1) sur des journaux synthétiques : python3 -m unittest trueglide/outils/test_centrage.py"""
import os, sys, unittest
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import analyse_blackbox as ab  # noqa: E402


def vol(duree_a=15.0, duree_b=15.0, vent=0.0, incl=0.0, coup=False, volts=15.6, gaz=1300.0):
    """Stationnaire à 1 kHz : cap A puis demi-tour (2 s à 90 °/s) puis cap B. Le centre de gravité est en avant :
    les moteurs avant tournent 3 % plus vite. Le vent ajoute +vent % aux moteurs face au vent (s'inverse avec le cap)."""
    dt = 0.001
    seq = [('arme', 3.0, 0), ('A', duree_a, 0), ('tour', 2.0, 90.0), ('B', duree_b, 0), ('fin', 3.0, 0)]
    t, cap_ph, lacet = [], [], []
    tt = 0.0
    for nom, d, rate in seq:
        k = int(d / dt)
        t += list(tt + np.arange(k) * dt); cap_ph += [nom] * k; lacet += [rate] * k; tt += k * dt
    t = np.array(t); n = len(t); lacet = np.array(lacet)
    base = np.array([10000.0, 10300.0, 10000.0, 10300.0])          # arrière droit, avant droit, arrière gauche, avant gauche
    signe = np.array([1 if p == 'A' else -1 for p in cap_ph])[:, None]
    vent_m = np.array([-1, 1, -1, 1]) * vent / 100                   # vent de face sur le cap A
    rpm = base * (1 + signe * vent_m) + np.random.default_rng(1).normal(0, 20, (n, 4))
    rc = np.zeros((n, 2))
    if coup:
        rc[int(5 / dt):int(5.2 / dt), 1] = 120
    acc = np.tile([np.sin(np.radians(incl)) * 2048, 0, np.cos(np.radians(incl)) * 2048], (n, 1))
    arme = np.ones(n, bool); arme[-10:] = False
    return ab.segments_centrage(t, arme, rc, np.full(n, gaz), rpm, np.full((n, 4), 30.0), np.full(n, volts),
                                lacet, acc, 4, np.zeros(n, bool))


class Centrage(unittest.TestCase):
    def test_vol_conforme(self):
        b = ab.bloc_centrage(vol(vent=2.0), ab.POSITIONS_QUADX, 4)
        self.assertEqual(b['refus'], [])
        self.assertEqual(b['protocole'], 'centrage_v1')
        # la moyenne des deux caps retire le vent : on retrouve le régime sans vent (avant 3 % plus vite)
        self.assertAlmostEqual(b['tr_min']['avant_droit'] / b['tr_min']['arriere_droit'], 1.03, places=2)
        self.assertGreaterEqual(b['caps']['A']['duree_s'], 10)
        self.assertGreaterEqual(b['caps']['B']['duree_s'], 10)
        self.assertEqual(b['ordre_moteurs']['1'], 'arriere_droit')

    def test_refus(self):
        self.assertTrue(any('un seul cap' in r for r in ab.bloc_centrage(vol(duree_b=0.5), ab.POSITIONS_QUADX, 4)['refus']))
        self.assertTrue(any('il en faut 20' in r for r in ab.bloc_centrage(vol(8, 8), ab.POSITIONS_QUADX, 4)['refus']))
        self.assertTrue(any('inclinaison' in r for r in ab.bloc_centrage(vol(incl=5), ab.POSITIONS_QUADX, 4)['refus']))
        self.assertEqual(ab.bloc_centrage(vol(volts=14.0), ab.POSITIONS_QUADX, 4)['refus'][0], 'aucune fenêtre de stationnaire calme')
        self.assertIsNone(ab.bloc_centrage(vol(), None, 4)['protocole'])

    def test_drone_pose_au_ralenti_ecarte(self):
        self.assertEqual(vol(gaz=1000.0), [])

    def test_coup_de_manche_ecarte(self):
        self.assertLess(len(vol(coup=True)), len(vol()))


if __name__ == '__main__':
    unittest.main()

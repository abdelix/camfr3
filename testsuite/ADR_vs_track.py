#!/usr/bin/env python

####################################################################
#
# The ADR solver (contour integrals + polyroot + Mueller) against the
# default track solver: two independent algorithms must give the same
# modes. A PML-terminated dielectric slab (TE) and a lossy metal film
# (TM, plasmonic modes).
#
####################################################################

from camfr import *

import unittest, eps

def modes(solver, layers):
    set_solver(solver)
    s = Slab(layers())
    s.calc()
    n = [s.mode(i).n_eff() for i in range(N())]
    free_tmps()
    return n

class ADR_vs_track(unittest.TestCase):
    def testADRvstrack(self):

        """ADR vs track"""

        print()
        print("Running ADR vs track...")

        set_lambda(1.55)
        set_N(12)

        # PML-terminated dielectric slab, TE.

        set_polarisation(TE)
        set_lower_PML(-0.1)
        set_upper_PML(-0.1)
        core, clad = Material(3.4), Material(1.45)
        layers = lambda: clad(1.0) + core(0.3) + clad(1.0)
        ADR_pml, track_pml = modes(ADR, layers), modes(track, layers)
        set_lower_PML(0)
        set_upper_PML(0)

        # Lossy metal film, TM.

        set_polarisation(TM)
        metal, diel = Material(0.2-10j), Material(1.5)
        layers = lambda: diel(1.0) + metal(0.05) + diel(1.0)
        ADR_met, track_met = modes(ADR, layers), modes(track, layers)

        set_solver(track)
        set_polarisation(TE)

        print(ADR_pml[0], "expected", track_pml[0])
        print(ADR_met[0], "expected", track_met[0])

        for a, t in zip(ADR_pml + ADR_met, track_pml + track_met):
            self.assertLess(abs((a - t) / t), eps.testing_eps)

suite = unittest.defaultTestLoader.loadTestsFromTestCase(ADR_vs_track)

if __name__ == "__main__":
    unittest.main()

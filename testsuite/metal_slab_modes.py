#!/usr/bin/env python

####################################################################
#
# Propagating modes of a metal/air slab (the 'cen' slab of
# metal_splitter.py), against reference values from Palace (FEM,
# tools/palace_metal_splitter/). The air gaps between the metal
# layers (epsr = -100) guide the light, so the slab solver needs
# set_low_index_core(True); with the default it misses both modes.
#
####################################################################

from camfr import *

import unittest, eps

class metal_slab_modes(unittest.TestCase):
    def testmodes(self):

        """Metal slab modes"""

        print()
        print("Running metal slab modes...")

        set_lambda(1.5)
        set_N(60)
        set_polarisation(TE)
        set_lower_wall(slab_H_wall)
        set_upper_PML(0)
        set_lower_PML(0)
        set_low_index_core(True)

        air = Material(1.0)
        met = Material(-10j)

        a, r = 0.600, 0.075
        cen = Slab(air(a-r) + 5*(met(2*r) + air(a-2*r)) + air(1.0))
        cen.calc()

        n = sorted([cen.mode(i).n_eff().real for i in range(N())
                    if cen.mode(i).n_eff().real > 0.05
                    and abs(cen.mode(i).n_eff().imag) < 1e-6], reverse=True)
        n_OK = [0.860825, 0.730098]  # Palace, order 3, mesh converged

        print(n, "expected", n_OK)
        passed = (len(n) == len(n_OK)) and all(
            abs((x - x_OK) / x_OK) < eps.testing_eps for x, x_OK in zip(n, n_OK))

        free_tmps()

        set_lower_wall(slab_E_wall)
        set_low_index_core(False)

        self.assertTrue(passed)

suite = unittest.defaultTestLoader.loadTestsFromTestCase(metal_slab_modes)

if __name__ == "__main__":
    unittest.main()

#!/usr/bin/env python

####################################################################
#
# Object lifetimes: C++ objects keep raw pointers to the objects they
# are built from. The bindings must keep those alive, e.g. for
# Stack(wg(0) + Slab(air(4.5))(1.0) + wg(0)) built from temporaries.
# Before pybind11 keep_alive this segfaulted.
#
####################################################################

from camfr import *

import gc, unittest, eps

def _stack_from_temporaries():
    air = Material(1.0)                  # freed when this returns
    si  = Material(3.5)
    wg  = Slab(air(2) + si(0.5) + air(2))
    return Stack(wg(0) + Slab(air(4.5))(1.0) + wg(0))

def _mode_of_temporary_slab():
    air = Material(1.0)
    si  = Material(3.5)
    slab = Slab(air(2) + si(0.5) + air(2))
    slab.calc()
    return slab.mode(0)                   # the mode must keep slab alive

# The same structures with every object kept alive, as reference. (The
# expected values depend on global settings left by earlier tests.)

air_ref, si_ref = Material(1.0), Material(3.5)

class lifetime(unittest.TestCase):
    def testlifetime(self):

        """Object lifetimes"""

        print()
        print("Running object lifetimes...")

        set_lambda(1.55)
        set_N(10)
        set_polarisation(TE)

        wg_ref    = Slab(air_ref(2) + si_ref(0.5) + air_ref(2))
        space_ref = Slab(air_ref(4.5))
        s_ref = Stack(wg_ref(0) + space_ref(1.0) + wg_ref(0))
        s_ref.calc()
        R_OK = abs(s_ref.R12(0,0))
        wg_ref.calc()
        n_OK = wg_ref.mode(0).n_eff()

        s = _stack_from_temporaries()
        m = _mode_of_temporary_slab()
        gc.collect()
        junk = [Material(float(i)) for i in range(2000)] # reuse memory

        s.calc()
        R = abs(s.R12(0,0))
        n = m.n_eff()

        print(R, "expected", R_OK)
        print(n, "expected", n_OK)
        passed =     abs((R - R_OK) / R_OK) < eps.testing_eps \
                 and abs((n - n_OK) / n_OK) < eps.testing_eps

        free_tmps()

        self.assertTrue(passed)

suite = unittest.defaultTestLoader.loadTestsFromTestCase(lifetime)

if __name__ == "__main__":
    unittest.main()

#!/usr/bin/env python

####################################################################
#
# Patterson quadrature (camfr/math/calculus/quadrature): the formula
# with 2**k - 1 nodes integrates polynomials up to degree
# 3*2**(k-1) - 1 exactly, and the adaptive sequence converges on
# smooth integrands. Also the complex moments used by the ADR solver
# (croot/patterson_z_n). Guards the MATH77-based implementation
# (PORTING_JOURNAL.md, entry 64).
#
####################################################################

import math
import unittest

import numpy as np
from numpy.polynomial import Legendre

import camfr._camfr as ext

class patterson(unittest.TestCase):
    def testexactness(self):

        """Patterson formulas: polynomial degree"""

        print()
        print("Running Patterson quadrature...")

        # eps = 0 never converges: the result is that of formula max_k.
        # Legendre polynomials integrate to 0 on [-1,1] and stay O(1) on
        # it, so a formula's error on them is not hidden by round-off.
        for k in range(2, 9):
            degree = 3 * 2**(k - 1) - 1      # odd; exact up to this
            for d in (degree - 1, degree + 1):
                P = Legendre.basis(d)
                r, not_converged, calls = ext._patterson(
                    lambda x: float(P(x)), -1.0, 1.0, 0.0, k)
                self.assertTrue(not_converged)
                self.assertEqual(calls, 2**k - 1)   # samples are reused
                if d < degree:
                    self.assertLess(abs(r), 1e-14, (k, d))
                elif k <= 6:  # beyond, the error is below round-off
                    self.assertGreater(abs(r), 1e-11, (k, d))

        # The 127- and 255-node formulas, on a Runge function.
        exact = 0.2 * math.atan(10.0)
        errors = [abs(ext._patterson(lambda x: 1 / (1 + 100 * x * x),
                                     -1.0, 1.0, 0.0, k)[0] - exact)
                  for k in (7, 8)]
        self.assertTrue(1e-11 < errors[0] < 1e-9, errors)
        self.assertLess(errors[1], 1e-15)

    def testconvergence(self):

        """Patterson formulas: convergence"""

        for f, a, b, exact in ((math.exp, 0.0, 1.0, math.e - 1),
                               (math.sin, 0.0, math.pi, 2.0),
                               (lambda x: 1 / (1 + 25 * x * x), -1.0, 1.0,
                                0.4 * math.atan(5.0))):
            r, not_converged, calls = ext._patterson(f, a, b, 1e-12)
            self.assertFalse(not_converged)
            self.assertLess(abs(r - exact), 1e-11 * abs(exact))

    def testmoments(self):

        """Patterson formulas: complex moments"""

        a, b, M = 1 + 1j, -0.5 + 2j, 5

        # 1/f = 1: the integrals of z**n are polynomial, hence exact.
        r, not_converged = ext._patterson_z_n(lambda z: 1.0, a, b, M, 1e-12)
        exact = [(b**(n + 1) - a**(n + 1)) / (n + 1) for n in range(M + 1)]
        self.assertFalse(not_converged)
        np.testing.assert_allclose(r, exact, rtol=1e-13)

        # 1/f = exp(z): the integral of exp(z) is exp(b) - exp(a).
        r, not_converged = ext._patterson_z_n(lambda z: np.exp(-z), a, b, 0,
                                              1e-12)
        self.assertFalse(not_converged)
        self.assertLess(abs(r[0] - (np.exp(b) - np.exp(a))), 1e-11)

suite = unittest.defaultTestLoader.loadTestsFromTestCase(patterson)

if __name__ == "__main__":
    unittest.main()

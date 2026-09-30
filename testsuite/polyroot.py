#!/usr/bin/env python

####################################################################
#
# polyroot (math/calculus/polyroot): roots of a complex polynomial as
# the eigenvalues of its companion matrix. Used by the contour-integral
# root finder (ADR solver, open slabs, Circ backward modes). Replaced
# Jenkins-Traub / ACM Algorithm 419 (PORTING_JOURNAL.md, entry 56).
#
####################################################################

import numpy as np
import camfr._camfr as _camfr

import unittest

def match(roots, true):
    """Largest relative distance after pairing each true root with its nearest."""
    roots = list(roots)
    worst = 0.0
    for t in sorted(true, key=abs, reverse=True):
        i = int(np.argmin([abs(r - t) for r in roots]))
        worst = max(worst, abs(roots.pop(i) - t) / max(abs(t), 1e-300))
    return worst

def backward_error(roots, coef):
    return np.linalg.norm(coef[0] * np.poly(roots) - coef) / np.linalg.norm(coef)

class polyroot(unittest.TestCase):

    def check(self, true, fwd_tol, coef=None):
        coef = np.poly(true).astype(complex) if coef is None else coef
        r = _camfr._polyroot(coef)
        self.assertEqual(len(r), len(true))
        self.assertLess(match(r, true), fwd_tol)
        self.assertLess(backward_error(r, coef), 1e-13)

    def test_integers(self):
        """Roots 1..10 (ill-conditioned)"""
        self.check(np.arange(1, 11) + 0j, 1e-9)

    def test_random(self):
        """Random complex roots, degree 1 to 10"""
        rng = np.random.default_rng(0)
        for _ in range(500):
            n = int(rng.integers(1, 11))
            true = rng.uniform(0, 1, n)**0.5 * np.exp(2j*np.pi*rng.uniform(0, 1, n))
            self.check(true, 1e-8)

    def test_repeated(self):
        """Double and triple roots (accuracy ~ eps^(1/multiplicity))"""
        self.check(np.array([0.7+0.2j, 0.7+0.2j, -0.3j, 1.1]), 1e-6)
        self.check(np.array([0.5j, 0.5j, 0.5j, -0.4+0.1j]), 1e-4)

    def test_zero_roots(self):
        """Roots at z = 0 (trailing zero coefficients)"""
        self.check(np.array([0, 0, 1+1j, -2]), 1e-12)

    def test_magnitudes(self):
        """Roots from 1e-3 to 1e3 in magnitude"""
        true = np.logspace(-3, 3, 10) * np.exp(1j*np.linspace(0, 6, 10))
        self.check(true, 1e-12)

    def test_degenerate_inputs(self):
        """Leading zero coefficients are skipped; constants have no roots"""
        r = _camfr._polyroot(np.array([0, 0, 1, -3, 2], complex))
        self.assertLess(match(r, [1, 2]), 1e-14)
        self.assertEqual(len(_camfr._polyroot(np.array([5], complex))), 0)
        self.assertEqual(len(_camfr._polyroot(np.array([0, 5], complex))), 0)
        self.assertEqual(len(_camfr._polyroot(np.array([], complex))), 0)

suite = unittest.defaultTestLoader.loadTestsFromTestCase(polyroot)

if __name__ == "__main__":
    unittest.main()

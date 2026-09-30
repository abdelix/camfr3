#!/usr/bin/env python

####################################################################
#
# Enum values: exported at module level, str() gives the bare name
# (as with Boost.Python), and they behave as integers. Guards the
# pybind11 bindings (PORTING_JOURNAL.md, entries 38 and 55).
#
####################################################################

from camfr import *
import camfr

import unittest

class enum_names(unittest.TestCase):
    def testnames(self):

        """Enum names"""

        print()
        print("Running enum names...")

        names = ["TE", "TM", "TEM", "ADR", "track", "series", "Plus", "Min",
                 "normal", "SVD", "E_wall", "H_wall", "L", "NT", "none",
                 "full", "GEV", "lapack", "cos_type", "identical",
                 "highest_index"]

        for n in names:
            v = getattr(camfr, n)
            self.assertEqual(str(v), n)
            self.assertEqual(f"{v}", n)

        self.assertEqual(int(TE), 2)
        self.assertTrue(TE == camfr.Polarisation.TE)
        self.assertEqual(repr(TE), "<Polarisation.TE: 2>")

suite = unittest.defaultTestLoader.loadTestsFromTestCase(enum_names)

if __name__ == "__main__":
    unittest.main()

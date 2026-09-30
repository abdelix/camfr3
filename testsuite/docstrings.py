#!/usr/bin/env python

####################################################################
#
# Every public function, class and method of the extension has a
# docstring, and arguments can be passed by keyword. The API
# reference of the documentation is generated from these docstrings
# (PORTING_JOURNAL.md, entry 61).
#
####################################################################

# No 'from camfr import *': it shadows builtins such as any() with NumPy's.

import camfr
import camfr._camfr as ext

import types

import unittest

def _described(f):
    # pybind11 puts the signature(s) first; a docstring adds text that
    # is not a signature or the "Overloaded function." header.
    lines = [l.strip() for l in (f.__doc__ or "").splitlines()]
    return any(l and not l.startswith(f.__name__ + "(")
               and not l[0].isdigit() and l != "Overloaded function."
               for l in lines)

class docstrings(unittest.TestCase):
    def testdocstrings(self):

        """Docstrings"""

        print()
        print("Running docstrings...")

        missing = []
        for name in dir(ext):
            if name.startswith("_"):
                continue
            obj = getattr(ext, name)
            if isinstance(obj, type):
                if not (obj.__doc__ or "").strip():
                    missing.append(name)
                for attr, f in vars(obj).items():
                    if attr.startswith("_") or not callable(f):
                        continue
                    if not _described(f):
                        missing.append(name + "." + attr)
            elif isinstance(obj, types.BuiltinFunctionType):
                if not _described(obj):
                    missing.append(name)

        self.assertEqual(missing, [])

    def testkeywords(self):

        """Keyword arguments"""

        old_N = camfr.N()
        camfr.set_N(N=7)
        self.assertEqual(camfr.N(), 7)
        camfr.set_N(old_N)

        m = camfr.Material(n=1.5)
        self.assertEqual(m.n(), 1.5)
        self.assertEqual(repr(camfr.Coord(c1=1, c2=2, z=3)),
                         repr(camfr.Coord(1, 2, 3)))

suite = unittest.defaultTestLoader.loadTestsFromTestCase(docstrings)

if __name__ == "__main__":
    unittest.main()

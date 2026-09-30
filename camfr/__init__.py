# Imports.



import numpy as np

# 'from camfr import *' has always provided the NumPy namespace, which used to
# come from 'from pylab import *'. Import the same NumPy names directly, so
# that Matplotlib is not loaded until something is plotted. As in pylab, the
# builtins below are not shadowed by their NumPy versions.

from numpy import *
from numpy.fft import *
from numpy.random import *
from numpy.linalg import *
import builtins as _builtins
bytes, abs, bool, max, min, pow, round = (_builtins.bytes, _builtins.abs,
  _builtins.bool, _builtins.max, _builtins.min, _builtins.pow, _builtins.round)
del _builtins

from ._camfr import *
from .camfr_PIL import *     # converted numpy* to np.*
from .geometry import *      # converted numpy* to np.*
from .geometry3d import *    # converted numpy* to np.*
from .material import *
from .section_matplotlib import *    # matplotlib functions for Section objects

# The version is written to _version.py by setuptools-scm at build time. It is
# missing in an in-place SCons build, which has no packaging step.

try:
    from ._version import __version__
except ImportError:
    __version__ = "0+unknown"

camfr_version = __version__  # name used before setuptools-scm

# Splash screen.

print()
print("CAMFR", camfr_version, end=' ')
print("- Copyright (C) 1998-2007 Peter Bienstman - Ghent University.")
print()

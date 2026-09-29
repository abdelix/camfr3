# Imports.



import numpy as np

import os
if not os.environ.get('NO_CAMFR_GRAPHICS'):
    from pylab import *

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

#!/usr/bin/env python3

# Build and install CAMFR.
#
# The compiled extension _camfr.so is built with SCons, using the settings in
# machine_cfg.py (copy machine_cfg.py.linux first). The
# package metadata and the version (setuptools-scm, from git tags) are in
# pyproject.toml.
#
#     cp machine_cfg.py.linux machine_cfg.py
#     uv sync                  # or: python3 -m pip install .

import os
import subprocess
import sys

from setuptools import Distribution, setup
from setuptools.command.build_py import build_py

# Build the extension with SCons before collecting the package files.

class camfr_build_py(build_py):

  def run(self):

    if not os.path.exists("machine_cfg.py"):
      sys.exit("machine_cfg.py not found: copy machine_cfg.py.linux to "
               "machine_cfg.py and edit it for your system.")

    subprocess.check_call([sys.executable, "-m", "SCons",
                           "-j", str(os.cpu_count() or 1)])

    return build_py.run(self)



# The package contains a compiled library, so wheels must be platform
# specific.

class camfr_distribution(Distribution):

  def has_ext_modules(self):
    return True



# Set up the module. The project metadata (name, version, dependencies) is
# in pyproject.toml; only the build customisation stays here.

setup(packages         = ["camfr"],
      package_data     = {"camfr": ["_camfr.so"]},
      distclass        = camfr_distribution,
      cmdclass         = {"build_py": camfr_build_py},
      zip_safe         = False,
      )

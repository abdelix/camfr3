#!/usr/bin/env python3

# Build and install CAMFR.
#
# The compiled extension _camfr.so is built with SCons, using the settings in
# machine_cfg.py (copy one of the machine_cfg.py.* templates first). The
# installed 'camfr' package is assembled from camfr/, the visualisation
# modules and camfrversion.py.
#
#     cp machine_cfg.py.linux machine_cfg.py
#     python3 -m pip install .

import os
import subprocess
import sys

from setuptools import Distribution, setup
from setuptools.command.build_py import build_py

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from camfrversion import camfr_version

# Modules that live outside camfr/ in the source tree but are installed
# inside the camfr package.

extra_modules = ["camfrversion.py",
                 "visualisation/camfr_PIL.py",
                 "visualisation/camfr_matlab.py",
                 "visualisation/camfr_tk.py",
                 "visualisation/section_matplotlib.py",
                 "visualisation/slab_plot.py",
                 "visualisation/stack_plot.py",
                 "visualisation/TkPlotCanvas.py",
                 "visualisation/matrix_plot_canvas.py",
                 "visualisation/gifmaker.py"]



# Build the extension with SCons before collecting the package files.

class camfr_build_py(build_py):

  def run(self):

    if not os.path.exists("machine_cfg.py"):
      sys.exit("machine_cfg.py not found: copy one of the machine_cfg.py.* "
               "templates to machine_cfg.py and edit it for your system.")

    subprocess.check_call([sys.executable, "-m", "SCons",
                           "-j", str(os.cpu_count() or 1)])

    return build_py.run(self)

  def find_package_modules(self, package, package_dir):

    modules = build_py.find_package_modules(self, package, package_dir)

    if package == "camfr":
      for path in extra_modules:
        name = os.path.splitext(os.path.basename(path))[0]
        modules.append((package, name, path))

    return modules



# The package contains a compiled library, so wheels must be platform
# specific.

class camfr_distribution(Distribution):

  def has_ext_modules(self):
    return True



# Set up the module.

# Published as 'camfr3' because the PyPI name 'camfr' belongs to the original
# author. The import name is still 'camfr'.

setup(name             = "camfr3",
      version          = camfr_version,
      description      = "CAvity Modelling FRamework, ported to Python 3",
      author           = "Peter Bienstman",
      author_email     = "Peter.Bienstman@UGent.be",
      maintainer       = "Abdelfettah Hadij-ElHouati",
      maintainer_email = "abdel.14@gmail.com",
      url              = "https://github.com/abdelix/CAMFR",
      project_urls     = {"Upstream": "https://github.com/demisjohn/CAMFR"},
      license          = "GPL-2.0-only",
      packages         = ["camfr"],
      package_data     = {"camfr": ["_camfr.so"]},
      python_requires  = ">=3.8",
      install_requires = ["numpy", "matplotlib", "pillow"],
      extras_require   = {"scipy": ["scipy"]},
      distclass        = camfr_distribution,
      cmdclass         = {"build_py": camfr_build_py},
      zip_safe         = False,
      )

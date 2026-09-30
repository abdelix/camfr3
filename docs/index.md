# camfr3

**camfr3** is CAMFR, the CAvity Modelling FRamework, for Python 3: a fast,
full-vectorial Maxwell solver based on eigenmode expansion (EME), with perfectly
matched layers (PML) and advanced boundary conditions. It computes

- the modes of slab, cylindrical and rectangular waveguides;
- the scattering matrices of stacks of waveguides, and the fields for any
  excitation;
- band diagrams of periodic structures;
- laser modes (threshold gain and wavelength) of cavities;
- the response to current sources, e.g. light emission from LEDs.

It is written in C++ and Fortran, and scripted from Python:

```python
from camfr import *

set_lambda(1.0)
set_N(20)

GaAs, air = Material(3.5), Material(1.0)
slab = Slab(air(2) + GaAs(0.5) + air(2))
slab.calc()
print(slab.mode(0).n_eff())      # (3.3979...+0j)
```

camfr3 is published as `camfr3` and imported as `camfr`. It is maintained by
Abdelfettah Hadij-ElHouati ([@abdelix](https://github.com/abdelix)); CAMFR was
written by Peter Bienstman and colleagues at Ghent University (see
[About](about.md)). Questions and bug reports go to the
[issue tracker](https://github.com/abdelix/camfr3/issues).

```{toctree}
:maxdepth: 2
:caption: User guide

install
tutorial
solvers
```

```{toctree}
:maxdepth: 2
:caption: Reference

api/index
```

```{toctree}
:maxdepth: 1
:caption: Project

about
Changelog <https://github.com/abdelix/camfr3/blob/main/CHANGELOG.md>
Source code <https://github.com/abdelix/camfr3>
```

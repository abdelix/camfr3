# CAMFR's modes of the metal_splitter slabs with the test's settings.
# Usage: python camfr_modes.py UPPER_PML   (e.g. -0.1 as in the test, or 0)
import sys, json
from camfr import *
from slabs import SLABS
pml = float(sys.argv[1])
set_lambda(1.5); set_N(60); set_polarisation(TE)
set_lower_wall(slab_H_wall); set_upper_PML(pml)
M = {"air": Material(1.0), "GaAs": Material(3.5), "met": Material(-10j)}
out = {}
for name, layers in SLABS.items():
    e = Expression()
    for m, d in layers: e.add(M[m](d))
    s = Slab(e); s.calc()
    out[name] = [[s.mode(i).n_eff().real, s.mode(i).n_eff().imag] for i in range(N())]
    print(f"{name:8s} width {s.width():.4f}  first modes:",
          ", ".join(f"{complex(*n):.4f}" for n in out[name][:6]), flush=True)
json.dump(out, open(f"camfr_pml{pml}.json", "w"))

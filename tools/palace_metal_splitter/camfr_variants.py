# Propagating modes of the metal slabs with one CAMFR setting changed.
# Usage: python camfr_variants.py "set_low_index_core(True)" UPPER_PML
import sys
from camfr import *
from slabs import SLABS
variant = sys.argv[1]
set_lambda(1.5); set_N(60); set_polarisation(TE)
set_lower_wall(slab_H_wall); set_upper_PML(float(sys.argv[2]))
exec(variant)
M = {"air": Material(1.0), "GaAs": Material(3.5), "met": Material(-10j)}
for name in ("cen", "arm", "ver"):
    e = Expression()
    for m, d in SLABS[name]: e.add(M[m](d))
    s = Slab(e); s.calc()
    prop = sorted((s.mode(i).n_eff() for i in range(N())
                   if s.mode(i).n_eff().real > 0.05 and abs(s.mode(i).n_eff().imag) < 0.05), key=lambda z: -z.real)
    print(f"  {name}: " + ", ".join(f"{n.real:.4f}" for n in prop), flush=True)

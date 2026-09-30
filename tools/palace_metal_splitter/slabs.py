# The slabs of testsuite/metal_splitter.py, shared by the scripts here.
# Layer definitions of the metal_splitter slabs, (material, thickness in um),
# from bottom (x = 0, magnetic wall) to top (electric wall, PML in CAMFR).
a, r, cl, periods, sections = 0.600, 0.075, 1.0, 3, 1
def rep(n, layers): return layers * n
SLABS = {
  "inc_wg":  [("GaAs", a/2), ("air", a - r + (sections+1+periods)*a + cl - a/2)],
  "no_rods": [("air", a - r + (sections+1+periods)*a + cl)],
  "cen":     [("air", a - r)] + rep(sections+1+periods, [("met", 2*r), ("air", a-2*r)]) + [("air", cl)],
  "arm":     [("met", r), ("air", a-2*r)] + rep(sections, [("met", 2*r), ("air", a-2*r)])
             + [("air", a)] + rep(periods, [("met", 2*r), ("air", a-2*r)]) + [("air", cl)],
  "ver":     [("air", a - r + (sections+1)*a)] + rep(periods, [("met", 2*r), ("air", a-2*r)]) + [("air", cl)],
}
EPS = {"air": 1.0, "GaAs": 3.5**2, "met": -100.0}   # CAMFR Material(-10j): n^2 = -100

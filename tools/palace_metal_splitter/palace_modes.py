# Modes of the metal_splitter slabs with Palace (2D BoundaryMode), as an
# independent check of CAMFR. Each 1D slab (layers along x) becomes a thin
# strip in (x, y) with PEC walls at y = 0 and y = h: fields uniform in y with
# only E_y are exactly CAMFR's TE slab modes. x = 0: PMC (CAMFR slab_H_wall);
# x = W: PEC (CAMFR's default upper wall; Palace has no PML here).
#
# Usage (needs gmsh in the Python environment and Palace):
#   PALACE=/path/to/palace/bin/palace python palace_modes.py SLAB TARGET NMODES H_METAL H_AIR
#   e.g. python palace_modes.py cen 0.7 8 0.004 0.02
# SLAB is a name from slabs.py, TARGET the n_eff around which modes are
# sought, H_METAL/H_AIR the mesh sizes (um) at the metal and elsewhere.
# Prints a JSON line with the n_eff; work files go to ./run_<slab>_...
import csv, json, os, subprocess, sys
from pathlib import Path
import gmsh
from slabs import SLABS, EPS

PALACE = os.environ.get("PALACE", "palace")   # path to the Palace launcher
wavelength = 1.5e-6
freq_ghz = 299792458.0 / wavelength / 1e9
h = 0.02                                   # strip height, um
name, target, nmodes, h_metal, h_air = sys.argv[1], float(sys.argv[2]), int(sys.argv[3]), float(sys.argv[4]), float(sys.argv[5])
layers = SLABS[name]
work = Path(f"run_{name}_t{target}_m{h_metal}"); work.mkdir(exist_ok=True)

gmsh.initialize(); gmsh.option.setNumber("General.Terminal", 0)
gmsh.model.add(name); occ = gmsh.model.occ
x, rects = 0.0, []
for m, d in layers:
    rects.append((occ.addRectangle(x, 0, 0, d, h), m)); x += d
W = x
occ.fragment([(2, t) for t, _ in rects[:1]], [(2, t) for t, _ in rects[1:]]) if len(rects) > 1 else None
occ.removeAllDuplicates(); occ.synchronize()
mat_ids = {"air": 1, "GaAs": 2, "met": 3}
groups = {k: [] for k in mat_ids}
for dim, tag in gmsh.model.getEntities(2):
    cx, _, _ = occ.getCenterOfMass(dim, tag)
    xs = 0.0
    for m, d in layers:
        if xs <= cx <= xs + d: groups[m].append(tag); break
        xs += d
for m, tags in groups.items():
    if tags: gmsh.model.addPhysicalGroup(2, tags, mat_ids[m], m)
pmc, pec = [], []
for dim, tag in gmsh.model.getEntities(1):
    cx, cy, _ = occ.getCenterOfMass(dim, tag)
    if abs(cx) < 1e-9: pmc.append(tag)
    elif abs(cx - W) < 1e-9 or abs(cy) < 1e-9 or abs(cy - h) < 1e-9: pec.append(tag)
gmsh.model.addPhysicalGroup(1, pmc, 1, "pmc"); gmsh.model.addPhysicalGroup(1, pec, 2, "pec")
# Mesh: fine in and next to metal (skin depth ~24 nm), coarser in air/GaAs.
gmsh.option.setNumber("Mesh.MeshSizeMax", h_air)
for dim, tag in gmsh.model.getEntities(2):
    if tag in groups["met"]:
        pts = gmsh.model.getBoundary([(dim, tag)], recursive=True)
        gmsh.model.mesh.setSize(pts, h_metal)
gmsh.option.setNumber("Mesh.MshFileVersion", 2.2)
gmsh.model.mesh.generate(2); gmsh.write(str(work / "slab.msh")); gmsh.finalize()

mats = [{"Attributes": [mat_ids[m]], "Permittivity": EPS[m], "Permeability": 1.0}
        for m, t in groups.items() if t]
config = {
  "Problem": {"Type": "BoundaryMode", "Verbose": 1, "Output": str(work / "postpro")},
  "Model": {"Mesh": str(work / "slab.msh"), "L0": 1.0e-6, "Refinement": {"MaxIts": 0}},
  "Domains": {"Materials": mats},
  "Boundaries": {"PEC": {"Attributes": [2]}, "PMC": {"Attributes": [1]}},
  "Solver": {"Order": 3,
             "BoundaryMode": {"Freq": freq_ghz, "N": nmodes, "Target": target, "Tol": 1e-9},
             "Linear": {"Tol": 1e-10}},
}
(work / "cfg.json").write_text(json.dumps(config, indent=1))
p = subprocess.run([PALACE, "-np", "2", str(work / "cfg.json")], capture_output=True, text=True)
(work / "palace.log").write_text(p.stdout + p.stderr)
if p.returncode:
    print("\n".join((p.stdout + p.stderr).splitlines()[-15:])); sys.exit(1)
rows = [r for r in csv.reader(open(work / "postpro" / "mode-kn.csv")) if r]
hdr = [c.strip() for c in rows[0]]
ire = next(i for i, c in enumerate(hdr) if "eff" in c and c.startswith("Re"))
iim = next((i for i, c in enumerate(hdr) if "eff" in c and c.startswith("Im")), None)
neffs = [complex(float(r[ire]), float(r[iim]) if iim is not None else 0.0) for r in rows[1:]]
print(json.dumps({"slab": name, "target": target, "h_metal": h_metal,
                  "neff": [[n.real, n.imag] for n in neffs]}))

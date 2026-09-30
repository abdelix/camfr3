# Palace check of the metal_splitter slab modes

Independent check of CAMFR's slab modes for `testsuite/metal_splitter.py` with
[Palace](https://awslabs.github.io/palace/) (FEM, 2D `BoundaryMode`). See
`PORTING_JOURNAL.md`, entry 46, for the results.

Each 1D CAMFR slab (layers along x) is modelled as a thin strip in (x, y) with
PEC walls at y = 0 and y = h, so the modes that are uniform in y with only E_y
are CAMFR's TE slab modes. x = 0 is PMC (CAMFR's `slab_H_wall`), x = W is PEC.
Palace's mode solver has no PML, so compare with CAMFR at `set_upper_PML(0)`.

```bash
# CAMFR (project venv)
../../.venv/bin/python camfr_modes.py 0
../../.venv/bin/python camfr_variants.py "set_low_index_core(True)" 0
# Palace (needs gmsh; e.g. the demultiplexers conda env)
PALACE=../../../demultiplexers/deps/palace/bin/palace \
  ~/anaconda3/envs/modesolvers/bin/python palace_modes.py cen 0.7 8 0.004 0.02
```

`testsuite/metal_slab_modes.py` keeps the Palace values for the `cen` slab as a
regression test.

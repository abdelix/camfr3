# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

CAMFR (CAvity Modelling FRamework) is a full-vectorial Maxwell solver built on eigenmode
expansion (EME): C++ and Fortran compiled into a Boost.Python extension (`camfr/_camfr.so`),
wrapped by a thin Python package.

Upstream (`master`) is Python 2.7 only. The `python3-port` branch ports it to Python 3.
**`PORTING_JOURNAL.md` documents every change, issue and resolution in numbered entries —
read it before touching the port, and add an entry for each new change.**

`MODERNISATION.md` is the checklist of planned build and packaging improvements
(CMake/scikit-build-core, pybind11, CI, wheels) and their suggested order. Check it before
changing the build, and tick items off as they land.

## Build and install

```bash
cp machine_cfg.py.linux machine_cfg.py          # machine_cfg.py itself is gitignored
BOOST_ROOT=$(realpath ../demultiplexers/deps/boost-root/usr) ../demultiplexers/deps/venv314/bin/pip install --no-build-isolation .
```

- `machine_cfg.py.linux` detects Python, NumPy and the versioned Boost.Python library
  (e.g. `libboost_python314`) from the interpreter running the build. `BOOST_ROOT` defaults
  to `/usr`; on this machine Boost was unpacked into `../demultiplexers/deps/boost-root/usr` (no root). It must be an absolute path: SCons resolves relative include paths from `camfr/`.
- To build in place without installing: `python3 -m SCons` (SCons drives the C++/Fortran build;
  `setup.py` calls it from `build_py`).
- The version comes from git tags via setuptools-scm (`v3.0.0a1` → `3.0.0a1`; untagged commits
  get `.devN+g<hash>`). With `--no-build-isolation` it must be installed in the venv (it is).
  The generated `camfr/_version.py` is gitignored; an in-place SCons build reports `0+unknown`.
- The venv once held both the old `camfr` and the new `camfr3` distribution, which own the same
  `camfr/` directory. Uninstall both before reinstalling if that happens again.
- Full build ≈ 70 s on 8 threads. Only `machine_cfg.py.linux` is maintained; the MacOSX/MSVC/
  gentoo templates are Python 2 era.

## Test

```bash
cd testsuite && MPLBACKEND=Agg ../../demultiplexers/deps/venv314/bin/python camfr_test.py   # 47 tests, expected: OK
```

Use the venv interpreter: `camfr` is installed there, not in the system Python. The testsuite
imports the *installed* package, so reinstall after changing sources.
Per-module runs are useful when a test crashes (one segfault aborts the whole suite). Each
test module defines a `suite` and runs standalone:
`cd testsuite && MPLBACKEND=Agg ../../demultiplexers/deps/venv314/bin/python wg.py`.
A new test must be added to both the import list and `alltests` in `camfr_test.py`.
Tests compare against hard-coded reference values with tolerance `eps.testing_eps`.
`ADR_solver`, `stack2` and `metal_splitter` fail; upstream excludes all three from
`camfr_test.py`, and it is unverified whether they ever passed on Python 2.

## Repository state

- Remotes: `origin` is the user's fork `abdelix/CAMFR`; `upstream` is `demisjohn/CAMFR`.
  `python3-port` tracks `origin/python3-port`. Never push to `upstream`.
- Tags `vX.Y.Z` set the package version (setuptools-scm); `v3.0.0a1` is pushed. Push new tags
  explicitly (`git push origin <tag>`).
- Pushing needs the user's credentials (HTTPS prompt), which this environment cannot supply:
  commit locally and ask the user to push from their own terminal.
- The upstream `py35_compat` branch is an earlier, abandoned attempt. It was not merged, and
  this port was done independently.
- `TODO` is Peter Bienstman's original feature wishlist. Leave it alone and track new work in
  `MODERNISATION.md`.

## Layout

- `camfr/` — C++/Fortran sources plus the Python package (`__init__.py`, `geometry*.py`,
  `material.py`, `RCLED.py`, `GARCLED.py`). `camfr_wrap*.cpp` are the Boost.Python bindings.
- `camfr/math/` — vendored numerics: SLATEC Bessel routines, Jenkins–Traub (ACM Algorithm 419),
  Brent root/minimum finders. **Do not reformat or "modernise" vendored files.**
- `visualisation/` — plotting modules; installed *into* the `camfr` package by `setup.py`.
- `testsuite/`, `examples/` — run from the source tree, not installed.

## Architecture

- **Two abstract hierarchies** carry the whole solver. `Waveguide` (`waveguide.h`) is a
  cross-section that finds its own eigenmodes (`find_modes`, `get_mode`). The concrete
  types are in `camfr/primitives/`: `planar` (1D, uniform), `slab` (2D, stratified),
  `circ` (cylindrical), `section` (3D rectangular, built from slabs) and `blochsection`.
  `Scatterer` (`scatterer.h`) maps modes of an incidence waveguide to modes of an exit
  waveguide via R/T matrices. `Interface`, `Stack`, `InfStack` and `BlochStack` are all
  scatterers.
- **Stacks are chains of `Chunk`s** (waveguide + length). The full structure's R/T come from
  the S-matrix cascade (`S_scheme.cpp`). Field calculation reuses those results through
  `S_scheme_fields`/`T_scheme_fields`.
  Interface overlap matrices are memoised in the global `interface_cache` (`icache.*`), and
  waveguides deregister from it in their destructor.
- **Global solver state.** Wavelength, number of modes `N`, polarisation, solver choice, PML
  and precision settings live in a single `Global global` struct (`defs.h`). Python sets them
  through free functions (`set_lambda`, `set_N`, `set_polarisation`, …). A change to them
  affects every object in the process. Call `free_tmps()` between independent calculations.
- **Python surface.** `camfr_wrap.cpp` (`BOOST_PYTHON_MODULE(_camfr)`) plus
  `camfr_wrap_2.cpp` (Cavity, Planar, Slab, Section, BlochSection) expose the C++ classes.
  `camfr/__init__.py` star-imports `_camfr`, the pure-Python geometry and material helpers
  and (unless `NO_CAMFR_GRAPHICS` is set) `pylab`. Expressions such as `Slab(air(2) + Si(0.5))`
  are built by `expression.*` from `Material(length)` terms.
- **Build.** `SConstruct` reads `machine_cfg.py` and delegates to `camfr/SConscript`. Object
  files (`*.os`) and `_camfr.so` land *in the source tree* (gitignored). The docs are
  Texinfo (`docs/camfr.texi`).

## Conventions

- **Commits:** Conventional Commits (`fix(wrap):`, `build:`, `style:`, `docs(journal):`), one
  self-contained change each, with a matching `PORTING_JOURNAL.md` entry in the same commit.
- **Style:** match the surrounding 1999-era C++ (2-space indent, banner comments). Python was
  converted mechanically with 2to3; tab indentation was expanded to spaces in a separate,
  behaviour-neutral commit.
- **Errors in C++:** use the existing idiom `py_error("..."); exit(-1);`.

## Gotchas

- **Keep waveguide objects alive.** `Term`/`Stack` hold raw C++ pointers, so
  `Stack(wg(0) + Slab(air(2))(0))` segfaults when the temporary `Slab` is freed. Bind slabs to
  variables. This is upstream behaviour, not a porting bug.
- **One `Section` solve per process.** Solving a second `Section` in the same interpreter
  segfaults (not yet investigated). Scripts take solver settings as CLI arguments instead.
- **Headless runs:** `NO_CAMFR_GRAPHICS=1` skips the pylab import; `MPLBACKEND=Agg` avoids Tk. Do not set `NO_CAMFR_GRAPHICS` for the testsuite: the tests use `zeros`/`arange` that only arrive through the `pylab` star-import (15 tests fail without it).
  Tk GUI plotting is unverified — there was no display server available.
- **Machine:** 4 physical cores / 8 threads. When running several CAMFR processes in parallel,
  set `OPENBLAS_NUM_THREADS=1` or they oversubscribe the CPU.
- **Killing processes:** use exact PIDs, not `pkill -f <pattern>`. The pattern can match the
  invoking shell and kill it (journal entry 23).
- **Imports:** modules inside the package are imported through it (`from camfr.RCLED import *`).
  The Python 2 installer's `camfr.pth` used to make them top-level.
- **`visualisation/camfr_matlab.py` is not ported** (needs `pymat`, Python 2 only).
- **Performance:** the `Section` solver spends ~90% of its time in the plane-wave estimation
  stage. Passing `Section.set_estimate(n_eff)` skips it and is ~40× faster for the same result.

## Open items

- **Published name is `camfr3`** (free on PyPI as of 2026-09-30; the module is still
  `import camfr`). `setup.py`, the README header, `NOTICE` and `CITATION.cff` are updated.
  Not yet uploaded — see the Distribution section of `MODERNISATION.md`.
- **Second-`Section` segfault** (see Gotchas): not yet investigated. An ASan/UBSan build is the
  suggested first step (`MODERNISATION.md`).
- **`examples/other/OLED_grating_avg.py`** was stopped before it finished (it is very long-running).
  It is not verified.

## Licensing

GPL v2 (`LICENSE`), plus an older permissive notice in `COPYRIGHT`; keep both, keep `AUTHORS`,
and mark modified files. Vendored ACM Algorithm 419 carries ACM's own terms (free for
non-commercial use) — check before any commercial distribution. The PyPI name `camfr` belongs to
the original author, so a rename is planned for publication; credit Bienstman & Baets,
*Opt. Quantum Electron.* **33**, 327–341 (2001).

## Context

This checkout was split out of the `demultiplexers` project (`../demultiplexers`), which
evaluates mode solvers for an AWG/demultiplexer modelling tool. The build dependencies
(Boost, the Python 3.14 venv) stay in `../demultiplexers/deps/`, shared with that project.
Benchmarks and comparison scripts are in `../demultiplexers/scripts/`;
CAMFR is one of five solvers compared there (femwell, Tidy3D, MPB, Palace).

Reference result, useful as a regression check: a 500 × 220 nm Si wire in SiO2 at 1550 nm
(n = 3.476 / 1.444). CAMFR gives neff TE0 2.4451, TM0 1.7702 and TE1 1.4925. Palace and
femwell agree to within 3e-4 (TE0 2.4454). The command is
(about 1 s):

```bash
NO_CAMFR_GRAPHICS=1 ../demultiplexers/deps/venv314/bin/python ../demultiplexers/scripts/si_wire_neff_camfr.py \
    --plane-waves 32 --slab-modes 70 --modes 3 --estimate 2.445 --estimate 1.770 --estimate 1.4925
```

Give exactly as many `--estimate`s as `--modes`. Surplus modes come back with zero field, and the
script crashes with `ZeroDivisionError`. Without estimates the same run takes about 200 s, and the
script's default 24/50 setting is coarser (TE0 2.4464).

The user's end goal is their own AWG/demux modelling tool, which needs PML, bent-waveguide modes
and nonuniform meshing. CAMFR is being kept alive as one candidate engine.

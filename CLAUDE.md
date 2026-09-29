# CLAUDE.md — working notes for CAMFR

CAMFR (CAvity Modelling FRamework) is a full-vectorial Maxwell solver built on eigenmode
expansion (EME): C++ and Fortran compiled into a Boost.Python extension (`camfr/_camfr.so`),
wrapped by a thin Python package.

Upstream (`master`) is Python 2.7 only. The `python3-port` branch ports it to Python 3.
**`PORTING_JOURNAL.md` documents every change, issue and resolution in 23 numbered entries —
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
- Full build ≈ 70 s on 8 threads. Only `machine_cfg.py.linux` is maintained; the MacOSX/MSVC/
  gentoo templates are Python 2 era.

## Test

```bash
cd testsuite && MPLBACKEND=Agg python3 camfr_test.py      # 47 tests, expected: OK
```

Per-module runs are useful when a test crashes (one segfault aborts the whole suite).
`ADR_solver`, `stack2` and `metal_splitter` fail; upstream excludes all three from
`camfr_test.py`, and it is unverified whether they ever passed on Python 2.

## Layout

- `camfr/` — C++/Fortran sources plus the Python package (`__init__.py`, `geometry*.py`,
  `material.py`, `RCLED.py`, `GARCLED.py`). `camfr_wrap*.cpp` are the Boost.Python bindings.
- `camfr/math/` — vendored numerics: SLATEC Bessel routines, Jenkins–Traub (ACM Algorithm 419),
  Brent root/minimum finders. **Do not reformat or "modernise" vendored files.**
- `visualisation/` — plotting modules; installed *into* the `camfr` package by `setup.py`.
- `testsuite/`, `examples/` — run from the source tree, not installed.

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
- **Imports:** modules inside the package are imported through it (`from camfr.RCLED import *`).
  The Python 2 installer's `camfr.pth` used to make them top-level.
- **`visualisation/camfr_matlab.py` is not ported** (needs `pymat`, Python 2 only).
- **Performance:** the `Section` solver spends ~90% of its time in the plane-wave estimation
  stage. Passing `Section.set_estimate(n_eff)` skips it and is ~40× faster for the same result.

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

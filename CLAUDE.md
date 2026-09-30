# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

CAMFR (CAvity Modelling FRamework) is a full-vectorial Maxwell solver built on eigenmode
expansion (EME): C++ and Fortran compiled into a pybind11 extension (`camfr/_camfr.so`),
wrapped by a thin Python package.

Upstream (`master`) is Python 2.7 only. The `python3-port` branch ports it to Python 3.
**`PORTING_JOURNAL.md` documents every change, issue and resolution in numbered entries —
read it before touching the port, and add an entry for each new change.**

`MODERNISATION.md` is the checklist of planned build and packaging improvements
(CMake/scikit-build-core, pybind11, CI, wheels) and their suggested order. Check it before
changing the build, and tick items off as they land.

## Build and install

The extension is built with CMake (`CMakeLists.txt`) through scikit-build-core; the
environment is managed with uv (`pyproject.toml`, `uv.lock`, `.python-version`):

```bash
uv sync      # creates .venv, builds camfr3 (isolated) and installs it editable
make dev     # after C++/Fortran edits: incremental rebuild in .venv (~7 s)
```

- **Rebuilds.** `uv sync` builds in a fresh isolated environment, and scikit-build-core then
  clears the CMake cache (by design), so a C++ change costs a full rebuild (~60 s on 8 threads).
  `make dev` (`uv pip install --no-build-isolation --no-deps -e .`) builds with the tools from
  the `dev` group in `.venv`, reusing `build/cp314-…` incrementally; afterwards `uv sync`/`uv run`
  accept that install and do not rebuild. The first `make dev` after a `uv sync` is full.
- **Editable install:** Python edits in `camfr/` take effect at once; the extension
  (`_camfr.cpython-314-….so`) and `_version.py` are installed in site-packages.
- **Bindings** use pybind11 (header-only, from PyPI; Boost.Python until journal entry 38).
  System libraries needed: Blitz++, BLAS/LAPACK, gfortran. An optional gitignored `local.cmake`
  (template `local.cmake.example`) holds machine-specific CMake settings; none are needed here.
  Do not add a `uv.toml`: uv then ignores `[tool.uv]` in `pyproject.toml`.
- `cmake` and `ninja` are also installed as uv tools in `~/.local/bin`; scikit-build-core uses
  them instead of downloading them for each isolated build.
- **Flags** match the old SCons build: C/C++ `-O3 -DNDEBUG`, Fortran `-O3`, and `defs.cpp`,
  `limits.c` and the two `camfr_wrap*.cpp` at `-O0`. Only `camfr_wrap.cpp` gets the NumPy
  include path.
- **Wheel contents** are controlled by `wheel.exclude` in `pyproject.toml` (`camfr/` also holds
  the C++/Fortran sources). Check with `uv build --wheel` + `unzip -l` after adding files.
- **Version** from git tags via setuptools-scm (`v3.0.0a1` → `3.0.0a1`; untagged commits get
  `.devN+g<hash>`), written to `camfr/_version.py` by scikit-build-core's `generate`.
- Without uv: `python3 -m pip install .` works (build deps, CMake and Ninja come from PyPI).
- **Warnings:** C/C++ compile with `-Wall -Wextra -Wno-unused-parameter` (option
  `CAMFR_WARNINGS`); Blitz++ headers are `SYSTEM`. ~318 unique warnings remain, mostly
  `sign-compare`/`reorder`; the ones that matter are listed in journal entry 37.
- **Sanitizers:** `make asan` installs an ASan+UBSan build (`CAMFR_SANITIZE`, RelWithDebInfo,
  `build/asan`) into `.venv` in place of the normal one; `make dev` switches back. Run with
  `LD_PRELOAD=$(gcc -print-file-name=libasan.so) ASAN_OPTIONS=detect_leaks=0
  UBSAN_OPTIONS=print_stacktrace=1:suppressions=$PWD/ubsan.supp` (absolute path). A full
  sanitizer build takes ~2.5 min.

## Test

```bash
cd testsuite && MPLBACKEND=Agg ../.venv/bin/python camfr_test.py   # 49 tests, expected: OK
```

Use the venv interpreter (or `uv run`): `camfr` is installed there, not in the system Python.
The install is editable, so rebuild (`make dev`) only after C++/Fortran changes.
Per-module runs are useful when a test crashes (one segfault aborts the whole suite). Each
test module defines a `suite` and runs standalone:
`cd testsuite && MPLBACKEND=Agg ../.venv/bin/python wg.py`.
A new test must be added to both the import list and `alltests` in `camfr_test.py`.
Tests compare against hard-coded reference values with tolerance `eps.testing_eps`.
Tests share CAMFR's global settings: a test that changes PML, walls or solver switches must
restore them (or the next test must set what it needs), or later tests fail only inside the
suite. `stack2` and `metal_splitter` still fail and stay out of `camfr_test.py` (entry 45):
both are deterministic and clean under ASan, but their results depend strongly on `N` and the
solver settings, so the stored expected values cannot be reproduced. `ADR_solver` passes since
the `polyroot` fix (entry 41) and is back in the suite.

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
  `material.py`, `RCLED.py`, `GARCLED.py`). `camfr_wrap*.cpp` are the pybind11 bindings.
- `camfr/math/` — vendored numerics: SLATEC Bessel routines, Jenkins–Traub (ACM Algorithm 419),
  Brent root/minimum finders. **Do not reformat or "modernise" vendored files.**
- `visualisation/` — only plotting examples now; the plotting modules live in `camfr/`.
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
- **Python surface.** `camfr_wrap.cpp` (`PYBIND11_MODULE(_camfr, m)`) plus
  `camfr_wrap_2.cpp` (Cavity, Planar, Slab, Section, BlochSection) expose the C++ classes;
  `camfr_wrap.h` holds the `cVector`/`cMatrix` ↔ NumPy type casters (copies; a `cVector`
  argument must be a 1D array of length `N()`). Pointer-returning methods use
  `return_value_policy::reference`. Optional constructor arguments are explicit overloads,
  because some C++ defaults (e.g. `Section`'s `M1`, `M2`) depend on `global` at call time.
  `camfr/__init__.py` star-imports the NumPy namespace (what `pylab` used to provide, minus
  Matplotlib), `_camfr` and the pure-Python helpers. Matplotlib, Pillow and tkinter are imported
  only when something is plotted (they are still required dependencies). Expressions such as `Slab(air(2) + Si(0.5))`
  are built by `expression.*` from `Material(length)` terms.
- **Build.** `CMakeLists.txt` builds the single module `camfr._camfr` from all C++/Fortran
  sources (lists taken over from the removed `camfr/SConscript`). Build products go to
  `build/` (gitignored), not the source tree. The docs are Texinfo (`docs/camfr.texi`).

## Conventions

- **Commits:** Conventional Commits (`fix(wrap):`, `build:`, `style:`, `docs(journal):`), one
  self-contained change each, with a matching `PORTING_JOURNAL.md` entry in the same commit.
- **Style:** match the surrounding 1999-era C++ (2-space indent, banner comments). Python was
  converted mechanically with 2to3; tab indentation was expanded to spaces in a separate,
  behaviour-neutral commit.
- **Errors in C++:** use the existing idiom `py_error("..."); exit(-1);`.

## Gotchas

- **Object lifetimes.** `Term`/`Expression`/`Stack`/`Slab`… hold raw C++ pointers to the objects
  they are built from. The bindings tie those lifetimes together with `py::keep_alive` (journal
  entry 39), so temporaries such as `Stack(wg(0) + Slab(air(2))(0))` are safe now (upstream
  segfaulted). Any new binding that stores a pointer to an argument needs a `keep_alive` too;
  `testsuite/lifetime.py` covers the pattern.
- **pybind11 must stay below 3.1** (pinned in `pyproject.toml`): 3.1.0 crashes in `keep_alive`
  on overloaded functions (entry 39). Re-run `testsuite/lifetime.py` before raising the pin.
- **Several `Section` solves per process** work (journal entry 44). A second solve used to
  segfault in the demultiplexers benchmarks; the only crashing pattern found is `Section`s built
  from a temporary `Slab`, which is a lifetime bug fixed in entry 39. The benchmark scripts
  still take one configuration per run; that is no longer necessary.
- **Headless runs:** `import camfr` no longer loads Matplotlib, so nothing is needed for
  computation-only scripts. `MPLBACKEND=Agg` avoids Tk when plotting. `NO_CAMFR_GRAPHICS` is
  obsolete and ignored (entry 34). `from camfr import *` still provides NumPy names (`zeros`,
  `arange`, …) but no longer pyplot names (`figure`, `savefig`, …): scripts that use those
  need `from pylab import *` or explicit Matplotlib imports.
  Tk GUI plotting is unverified — there was no display server available.
- **Machine:** 4 physical cores / 8 threads. When running several CAMFR processes in parallel,
  set `OPENBLAS_NUM_THREADS=1` or they oversubscribe the CPU.
- **Killing processes:** use exact PIDs, not `pkill -f <pattern>`. The pattern can match the
  invoking shell and kill it (journal entry 23).
- **Imports:** modules inside the package are imported through it (`from camfr.RCLED import *`).
  The Python 2 installer's `camfr.pth` used to make them top-level.
- **`camfr/camfr_matlab.py` is not ported** (needs `pymat`, Python 2 only).
- **Performance:** the `Section` solver spends ~90% of its time in the plane-wave estimation
  stage. Passing `Section.set_estimate(n_eff)` skips it and is ~40× faster for the same result.

## Open items

- **Published name is `camfr3`** (free on PyPI as of 2026-09-30; the module is still
  `import camfr`). `pyproject.toml`, the README header, `NOTICE` and `CITATION.cff` are updated.
  Not yet uploaded — see the Distribution section of `MODERNISATION.md`.
- **Sanitizer status:** the testsuite, and `stack2`/`metal_splitter`, run clean under
  ASan/UBSan (entries 37–45).
- **`stack2` and `metal_splitter`** (entry 45): numerically ill-conditioned. `metal_splitter`
  (a metal with epsr = −100) gives |R12| ≈ 1.72 > 1 with the default solver and anything from
  −0.08 to 0.89+0.40j with other settings; the solver does not find this structure's modes
  reliably. Needs solver work (and reference values from an independent method).
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
(the other solvers' venvs; also the Boost that camfr used before pybind11) stay in
`../demultiplexers/deps/`. The Python 3.14 venv
lives in this repo as `.venv/` (managed by uv, ignored by its own `.gitignore`);
`../demultiplexers/deps/venv314` is a symlink to it, so the benchmark scripts there keep working
and use the editable camfr from this checkout. `uv sync` recreates `.venv` at the same path, so
the link stays valid.
Benchmarks and comparison scripts are in `../demultiplexers/scripts/`;
CAMFR is one of five solvers compared there (femwell, Tidy3D, MPB, Palace).

Reference result, useful as a regression check: a 500 × 220 nm Si wire in SiO2 at 1550 nm
(n = 3.476 / 1.444). CAMFR gives neff TE0 2.4451, TM0 1.7702 and TE1 1.4925. Palace and
femwell agree to within 3e-4 (TE0 2.4454). The command is
(about 1 s):

```bash
.venv/bin/python ../demultiplexers/scripts/si_wire_neff_camfr.py \
    --plane-waves 32 --slab-modes 70 --modes 3 --estimate 2.445 --estimate 1.770 --estimate 1.4925
```

Give exactly as many `--estimate`s as `--modes`. Surplus modes come back with zero field, and the
script crashes with `ZeroDivisionError`. Without estimates the same run takes about 200 s, and the
script's default 24/50 setting is coarser (TE0 2.4464).

The user's end goal is their own AWG/demux modelling tool, which needs PML, bent-waveguide modes
and nonuniform meshing. CAMFR is being kept alive as one candidate engine.

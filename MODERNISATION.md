# Build and packaging modernisation

Checklist of recommended improvements to CAMFR's build, packaging and tooling after the
Python 3 port. Nothing here is started yet. Tick items off as they land, and record each
change in `PORTING_JOURNAL.md` as for the port itself.

Suggested order: **1 → 5 → 2 → 3 → 6**, with 4 whenever cross-platform wheels matter.
Step 5 (CI) comes early so that everything after it is guarded by tests.

## 1. Quick wins (hours each)

- [ ] Stop importing everything from `pylab` in `camfr/__init__.py`. It slows `import camfr`,
      floods the namespace with Matplotlib names, and is why `NO_CAMFR_GRAPHICS` exists.
      Import plotting lazily; make Matplotlib and Pillow optional extras (`[plot]`).
- [ ] Move the `visualisation/` modules into the package (`camfr/visualisation/` or `camfr/`),
      so the `find_package_modules` override in `setup.py` can go and the source tree matches
      the installed layout.
- [x] Replace the fixed version `20090406` in `camfrversion.py` with real versioning
      (e.g. `setuptools-scm` from git tags). Done in journal entry 29.
- [ ] Delete dead build paths: ~~the `distrib` target in `makefile`~~ (removed in entry 29),
      and the Python 2-era
      `machine_cfg.py.{MacOSX,MSVC,gentoo,gcc}` templates.
- [ ] Enable `-Wall -Wextra`, and add a debug build with AddressSanitizer and
      UndefinedBehaviorSanitizer. Both crashes fixed in the port (journal entries 16 and 19)
      would have been caught immediately.

## 2. Modernise the build system (days)

- [ ] Replace SCons + `machine_cfg.py` + the `setup.py` `build_py` hook with
      **scikit-build-core + CMake** (or meson-python):
  - declarative `pyproject.toml` + `CMakeLists.txt`; no hand-edited, gitignored config;
  - `find_package(Python)`, `find_package(LAPACK)`, `find_package(Boost COMPONENTS python)`
    instead of manual paths and guessing the `boost_python314` library name;
  - working editable installs (`pip install -e .`), correct wheel tags, first-class Fortran;
  - incremental builds, ccache, Ninja.

## 3. Replace Boost.Python with pybind11 or nanobind (1–2 weeks)

- [ ] Rewrite `camfr/camfr_wrap.cpp` and `camfr/camfr_wrap_2.cpp` (~1,400 lines):
  - removes the hardest dependency (Boost.Python for Python 3.14 had to be unpacked from
    Ubuntu packages by hand); pybind11 is header-only and pip-installable;
  - replaces the hand-written NumPy converters (broken by NumPy 2) with `py::array_t`;
  - use `py::keep_alive` to fix the "keep waveguide objects alive" segfault, e.g.
    `Stack(wg(0) + Slab(air(2))(0))`.

## 4. Remove the Fortran dependency (optional, a few days)

- [ ] Replace the Jenkins–Traub root finder (`jenkins_traub.f`, ACM Algorithm 419). This also
      removes ACM's non-commercial licence restriction, useful for a clean GPL release.
- [ ] Replace the vendored SLATEC/AMOS Bessel routines with a C++ port of the same algorithms,
      dropping gfortran entirely (much easier macOS/Windows wheels). Validate carefully: the
      cylindrical (`Circ`) solver depends on them.

## 5. Testing and CI (2–3 days)

- [ ] Convert `testsuite/` to pytest, each test module in its own process (e.g.
      `pytest-forked`), so one segfault does not abort the run.
- [ ] Mark `ADR_solver`, `stack2`, `metal_splitter` as `xfail` with a reason, instead of
      silently leaving them out of `camfr_test.py`.
- [ ] GitHub Actions on Linux and macOS: build, test, sanitizer job.
- [ ] `cibuildwheel` to publish binary wheels (manylinux + OpenBLAS), so users never compile.

## 6. Distribution

- [ ] Publish under the new name `camfr3` (the PyPI name `camfr` belongs to the original author).
      Metadata, `NOTICE` and `CITATION.cff` are done (journal entry 27); the upload is not.
- [ ] conda-forge feedstock (Boost, Blitz++ and OpenBLAS are already on conda-forge).

## 7. Related correctness work (not build issues)

- [ ] Investigate the segfault when a second `Section` is solved in the same process — likely
      another lifetime or cache bug like journal entries 16 and 19.
- [ ] Blitz++ is barely maintained; moving to Eigen is possible but a large refactor.
      Leave it unless it blocks something.

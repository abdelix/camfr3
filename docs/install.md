# Installation

camfr3 supports CPython 3.10 to 3.14.

## From PyPI (Linux)

On Linux x86_64, pip installs a pre-built wheel that contains everything,
including OpenBLAS and Blitz++, so nothing needs compiling. Releases are alpha
versions for now, which pip only installs with `--pre`:

```bash
python3 -m pip install --pre camfr3
```

CI also builds wheels for every commit: the `wheel-cpXYZ-manylinux_x86_64`
artifacts of a [CI run](https://github.com/abdelix/camfr3/actions/workflows/ci.yml)
(`cp312` for Python 3.12, …) install with `pip install camfr3-*.whl`.

## From source

The build needs a C++ and a Fortran compiler, Blitz++ and BLAS/LAPACK. The
Python build tools (CMake, Ninja, scikit-build-core, pybind11) are fetched
automatically. On Debian or Ubuntu:

```bash
sudo apt-get install g++ gfortran libblitz0-dev libblas-dev liblapack-dev
git clone https://github.com/abdelix/camfr3.git
cd camfr3
python3 -m pip install .
```

On Fedora the packages are `gcc-c++ gcc-gfortran blitz-devel openblas-devel`.
Machine-specific CMake settings (e.g. a Blitz++ in a non-standard place) go in
a `local.cmake` file; see `local.cmake.example`.

## For development

The repository uses [uv](https://docs.astral.sh/uv/):

```bash
uv sync                 # .venv with an editable install and the test tools
uv run pytest           # the testsuite
make dev                # incremental rebuild after C++/Fortran changes
make docs               # this documentation, in docs/_build/html
```

## Checking the installation

```bash
python3 -c "import camfr; print(camfr.__version__)"
```

`import camfr` prints a copyright banner. Matplotlib and Pillow are installed
with camfr3 but only imported when something is plotted.

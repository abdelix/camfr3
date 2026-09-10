# CAMFR → Python 3 porting journal

This journal records every change, issue and resolution made while porting CAMFR
from Python 2.7 to a current Python 3 toolchain.
Entries are in chronological order, and each one matches one or more commits on
the `python3-port` branch. Commit messages follow
[Conventional Commits](https://www.conventionalcommits.org/).

## Starting point

- Upstream: <https://github.com/demisjohn/CAMFR>, `master` at `f00a092` (2022-05-08).
  The package `camfrversion.py` still says `20090406`.
- `master` is Python 2.7 only. The README says "CAMFR currently only supports Python 2.7".
- A partial community attempt exists on `origin/py35_compat` (last commit 2022-05-08).
  Its commit messages admit to hand-edited indentation ("Maybe some loops/conditionals are
  screwed up still"). **Decision:** I branch from `master` and re-do the Python conversion
  mechanically (see entry 5). I only reuse ideas from `py35_compat` where they are useful,
  e.g. the `_import_array()` error check and `env.Copy` → `env.Clone`, and credit them.

## Target environment

| Component | Version | Source |
|---|---|---|
| OS | Ubuntu 26.04.1 LTS, x86_64 | – |
| Python | 3.14.4 (latest stable on this system) | `/usr/bin/python3` |
| NumPy | 2.3.5 | system `python3-numpy` |
| C/C++/Fortran | gcc / g++ / gfortran 15.2.0 | system |
| SCons | 4.8.1 | system |
| Blitz++ | 1.0.2 | system `libblitz0-dev` |
| BLAS / LAPACK | reference 3.12.1 | system |
| Boost.Python | 1.90.0, built for Python 3.14 (`libboost_python314`) | see entry 1 |

## Journal

### 1. Obtaining Boost.Python without root (environment, no commit)

**Issue.** Neither the Boost headers nor Boost.Python are installed. There is no
passwordless `sudo`, so `apt install libboost-python-dev` isn't possible from the
porting session.

**Resolution.** Ubuntu's own packages were downloaded and unpacked into a user-owned
prefix, outside the repository:

```bash
mkdir -p ../deps/debs && cd ../deps/debs
apt-get download libboost1.90-dev libboost-python1.90-dev libboost-python1.90.0
for d in *.deb; do dpkg -x "$d" ../boost-root; done
# headers: ../deps/boost-root/usr/include
# library: ../deps/boost-root/usr/lib/x86_64-linux-gnu/libboost_python314.so
```

On a machine where you have root, `sudo apt install libboost-python-dev` gives the same
library in `/usr`. The build configuration (entry 3) accepts either location.

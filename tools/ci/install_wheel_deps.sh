#!/bin/bash
# System dependencies for building the Linux wheels, run by cibuildwheel
# (before-all) inside the manylinux_2_28 container (AlmaLinux 8, which already
# has gfortran from gcc-toolset). auditwheel later copies the shared libraries
# (OpenBLAS, Blitz++, libgfortran) into each wheel.

set -euxo pipefail

# BLAS + LAPACK: OpenBLAS includes both (PowerTools repository).
dnf install -y openblas-devel

# Blitz++ is not packaged for AlmaLinux 8: build the last release from source.
# Its CMakeLists.txt (2019) predates CMake 3.5, which CMake 4 no longer
# accepts without CMAKE_POLICY_VERSION_MINIMUM.
BLITZ=1.0.2
curl -fsSL "https://github.com/blitzpp/blitz/archive/refs/tags/${BLITZ}.tar.gz" | tar -xz -C /tmp
cmake -S "/tmp/blitz-${BLITZ}" -B /tmp/blitz-build -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_DOC=OFF -DBUILD_TESTING=OFF \
      -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build /tmp/blitz-build --parallel "$(nproc)"
cmake --install /tmp/blitz-build

# The project is copied into the container with another owner; let git (and
# so setuptools-scm, which reads the version from the tags) use it.
git config --global --add safe.directory '*'

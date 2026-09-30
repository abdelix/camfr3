#############################################################################
#
# CMake build for the CAMFR extension module camfr._camfr.
#
# Driven by scikit-build-core (see pyproject.toml): 'uv sync' or
# 'pip install .'. Replaces the SCons build (SConstruct, camfr/SConscript,
# machine_cfg.py); the source lists and flags are taken over unchanged.
#
# Machine-specific settings (e.g. CMAKE_PREFIX_PATH for libraries outside the
# system prefixes) go in an optional, gitignored local.cmake next to this
# file.
#
#############################################################################

cmake_minimum_required(VERSION 3.26)
project(camfr LANGUAGES C CXX Fortran)

include(${CMAKE_CURRENT_SOURCE_DIR}/local.cmake OPTIONAL)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

#############################################################################
#
# Dependencies.
#
#############################################################################

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

# pybind11 (header-only) generates the bindings. scikit-build-core makes the
# pip-installed pybind11 visible to CMake.

find_package(pybind11 2.12 CONFIG REQUIRED)

find_path(BLITZ_INCLUDE_DIR blitz/array.h REQUIRED)
find_library(BLITZ_LIBRARY blitz REQUIRED)

find_package(LAPACK REQUIRED)   # also finds BLAS

#############################################################################
#
# Sources (as in the former camfr/SConscript).
#
#############################################################################

set(CAMFR_CXX_SOURCES
  material.cpp
  coord.cpp
  field.cpp
  mode.cpp
  waveguide.cpp
  scatterer.cpp
  chunk.cpp
  interface.cpp
  icache.cpp
  expression.cpp
  stack.cpp
  S_scheme.cpp
  T_scheme.cpp
  S_scheme_fields.cpp
  T_scheme_fields.cpp
  cavity.cpp
  bloch.cpp
  infstack.cpp
  util/cvector.cpp
  util/index.cpp
  util/tracesorter.cpp
  math/bessel/bessel.cpp
  math/linalg/linalg.cpp
  math/calculus/root/root.cpp
  math/calculus/croot/allroots.cpp
  math/calculus/croot/contour.cpp
  math/calculus/croot/patterson_z_n.cpp
  math/calculus/croot/mueller.cpp
  math/calculus/polyroot/polyroot.cpp
  math/calculus/traceroot/traceroot.cpp
  math/calculus/quadrature/patterson.cpp
  math/calculus/quadrature/patterson_quad.cpp
  math/calculus/minimum/minimum.cpp
  math/calculus/fourier/fourier.cpp
  primitives/planar/planar.cpp
  primitives/circ/circ.cpp
  primitives/circ/circmode.cpp
  primitives/circ/circdisp.cpp
  primitives/circ/circoverlap.cpp
  primitives/circ/circ_M_util.cpp
  primitives/slab/generalslab.cpp
  primitives/slab/slabmatrixcache.cpp
  primitives/slab/isoslab/slab.cpp
  primitives/slab/isoslab/slaboverlap.cpp
  primitives/slab/isoslab/slabdisp.cpp
  primitives/slab/isoslab/slabwall.cpp
  primitives/slab/isoslab/slabmode.cpp
  primitives/section/section.cpp
  primitives/section/sectionmode.cpp
  primitives/section/sectiondisp.cpp
  primitives/section/sectionoverlap.cpp
  primitives/section/refsection.cpp
  primitives/blochsection/blochsection.cpp
  primitives/blochsection/blochsectionmode.cpp
  primitives/blochsection/blochsectionoverlap.cpp)

set(CAMFR_NOOPT_SOURCES
  defs.cpp
  math/bessel/slatec/limits.c
  camfr_wrap.cpp
  camfr_wrap_2.cpp)

set(CAMFR_FORTRAN_SOURCES
  math/bessel/slatec/dyairy.f
  math/bessel/slatec/xerhlt.f
  math/bessel/slatec/zbesh.f
  math/bessel/slatec/zlog.f
  math/bessel/slatec/zuni1.f
  math/bessel/slatec/d1mach.f
  math/bessel/slatec/dbesy1.f
  math/bessel/slatec/fdump.f
  math/bessel/slatec/xermsg.f
  math/bessel/slatec/zbesj.f
  math/bessel/slatec/zmlri.f
  math/bessel/slatec/zuni2.f
  math/bessel/slatec/dbesy.f
  math/bessel/slatec/i1mach.f
  math/bessel/slatec/xerprn.f
  math/bessel/slatec/zbesy.f
  math/bessel/slatec/zmlt.f
  math/bessel/slatec/zunik.f
  math/bessel/slatec/d9b0mp.f
  math/bessel/slatec/dbsynu.f
  math/bessel/slatec/initds.f
  math/bessel/slatec/xersve.f
  math/bessel/slatec/zbinu.f
  math/bessel/slatec/zrati.f
  math/bessel/slatec/zunk1.f
  math/bessel/slatec/d9b1mp.f
  math/bessel/slatec/dcsevl.f
  math/bessel/slatec/j4save.f
  math/bessel/slatec/xgetua.f
  math/bessel/slatec/zbknu.f
  math/bessel/slatec/zs1s2.f
  math/bessel/slatec/zunk2.f
  math/bessel/slatec/d9lgmc.f
  math/bessel/slatec/dgamlm.f
  math/bessel/slatec/zabs.f
  math/bessel/slatec/zbuni.f
  math/bessel/slatec/zseri.f
  math/bessel/slatec/zuoik.f
  math/bessel/slatec/dasyjy.f
  math/bessel/slatec/dgamln.f
  math/bessel/slatec/zacai.f
  math/bessel/slatec/zbunk.f
  math/bessel/slatec/zshch.f
  math/bessel/slatec/zwrsk.f
  math/bessel/slatec/dbesj0.f
  math/bessel/slatec/dgamma.f
  math/bessel/slatec/zacon.f
  math/bessel/slatec/zdiv.f
  math/bessel/slatec/zsqrt.f
  math/bessel/slatec/dbesj1.f
  math/bessel/slatec/djairy.f
  math/bessel/slatec/zairy.f
  math/bessel/slatec/zexp.f
  math/bessel/slatec/zuchk.f
  math/bessel/slatec/dbesj.f
  math/bessel/slatec/dlngam.f
  math/bessel/slatec/xercnt.f
  math/bessel/slatec/zasyi.f
  math/bessel/slatec/zkscl.f
  math/bessel/slatec/zunhj.f
  math/bessel/slatec/dbesy0.f)
list(TRANSFORM CAMFR_CXX_SOURCES     PREPEND camfr/)
list(TRANSFORM CAMFR_NOOPT_SOURCES   PREPEND camfr/)
list(TRANSFORM CAMFR_FORTRAN_SOURCES PREPEND camfr/)

# These files were built without optimisation in the SCons build (the
# Python wrappers are slow to compile with -O3, and defs.cpp/limits.c
# hold machine constants). Keep it that way.

set_source_files_properties(${CAMFR_NOOPT_SOURCES}
                            PROPERTIES COMPILE_OPTIONS "-O0")

#############################################################################
#
# The extension module.
#
#############################################################################

Python_add_library(_camfr MODULE WITH_SOABI
                   ${CAMFR_CXX_SOURCES}
                   ${CAMFR_NOOPT_SOURCES}
                   ${CAMFR_FORTRAN_SOURCES})

# Third-party headers as SYSTEM, so that -Wall -Wextra reports CAMFR's code only.

target_include_directories(_camfr SYSTEM PRIVATE ${BLITZ_INCLUDE_DIR})

target_compile_definitions(_camfr PRIVATE
                           FORTRAN_SYMBOLS_WITH_SINGLE_TRAILING_UNDERSCORE)

target_link_libraries(_camfr PRIVATE
                      pybind11::headers
                      ${BLITZ_LIBRARY}
                      LAPACK::LAPACK)

# Hidden symbol visibility, as pybind11 recommends for extension modules.
# Non-system link directories (e.g. a local Blitz++) go into the rpath.

set_target_properties(_camfr PROPERTIES CXX_VISIBILITY_PRESET hidden
                                        VISIBILITY_INLINES_HIDDEN ON
                                        INSTALL_RPATH_USE_LINK_PATH TRUE)

#############################################################################
#
# Warnings and sanitizers.
#
#############################################################################

# Compiler warnings for the C and C++ sources (the vendored Fortran is left
# alone). -Wunused-parameter is off: the virtual interfaces have many
# deliberately unused parameters (~800 warnings of pure noise).

option(CAMFR_WARNINGS "Compile C/C++ with -Wall -Wextra" ON)
if(CAMFR_WARNINGS)
  target_compile_options(_camfr PRIVATE
    $<$<COMPILE_LANGUAGE:C,CXX>:-Wall -Wextra -Wno-unused-parameter>)
endif()

# Debug build with AddressSanitizer and UndefinedBehaviorSanitizer ('make
# asan'). The module is loaded into an uninstrumented Python, so run it with
# LD_PRELOAD=$(gcc -print-file-name=libasan.so) (see CLAUDE.md).

option(CAMFR_SANITIZE "Build with AddressSanitizer and UBSan" OFF)
if(CAMFR_SANITIZE)
  set(CAMFR_SANITIZE_FLAGS -fsanitize=address,undefined
                           -fno-omit-frame-pointer)
  target_compile_options(_camfr PRIVATE ${CAMFR_SANITIZE_FLAGS} -g)
  target_link_options(_camfr PRIVATE ${CAMFR_SANITIZE_FLAGS})
endif()

install(TARGETS _camfr LIBRARY DESTINATION camfr)

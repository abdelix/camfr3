# Solvers and settings

## Global state

CAMFR keeps its settings in one global state: the wavelength, the number of
modes, the polarisation, the solvers, PML and walls. The `set_*` functions
change it for everything that follows in the process. Some settings (PML,
walls) are read when a waveguide is **created**, others (wavelength, `N`,
solvers) when it is **calculated**. So set PML and walls before defining the
structures they apply to.

Waveguides and stacks cache their results and recalculate only when a relevant
setting changed. Interface matrices are kept in a global cache: call
{py:func}`~camfr.free_tmps` between independent calculations, or at the end of
an inner loop, to release them.

## Number of modes and convergence

`set_N(n)` sets the number of modes in each waveguide's expansion. Results
converge as `N` grows; check it by repeating a calculation with a larger `N`.
The field example of the [tutorial](tutorial.md#fields-in-a-stack) gives

| `N` | \|R\|² | \|T\|² |
|----:|-------:|-------:|
| 20  | 0.3588 | 0.1144 |
| 40  | 0.3557 | 0.1151 |
| 60  | 0.3557 | 0.1153 |

Computation time grows roughly as `N`³ (matrix operations), so symmetry that
halves the structure (below) also halves `N`, a factor of about eight.

## Boundaries: walls and PML

Slabs are enclosed by walls at `x = 0` and `x = width`, electric by default.
Change them for the slabs defined afterwards with
{py:func}`~camfr.set_lower_wall` / {py:func}`~camfr.set_upper_wall`
(`slab_E_wall`, `slab_H_wall`, `slab_no_wall`), or per slab with
`Slab.set_lower_wall` / `set_upper_wall`. Sections have
{py:func}`~camfr.set_left_wall` / {py:func}`~camfr.set_right_wall`
(`E_wall`, `H_wall`), and their top and bottom walls are those of their slabs.

**Symmetry.** A structure symmetric about a plane can be cut in half with a
wall on that plane. For TE, a magnetic wall (`slab_H_wall`) keeps the even
modes (including the fundamental) and an electric wall the odd ones; for TM it
is the other way round. Only the modes of that symmetry are then found, and no
PML is needed on the symmetry plane.

**PML.** Closed walls reflect everything that leaves the waveguide, and the
modes of a closed box can sit exactly at cutoff (CAMFR warns "mode close to
cutoff"). Open structures therefore need perfectly matched layers:
`set_lower_PML(p)` gives the first layer of each subsequent slab an imaginary
thickness `p*1j`; `p` is negative (absorbing), typically -0.01 to -0.1.
Likewise `set_upper_PML` (last layer), `set_left_PML` / `set_right_PML`
(Sections) and `set_circ_PML` (the cladding of `Circ`). The PML must be thick
enough, and the cladding it sits on far enough from the core, not to disturb
the guided modes; vary both to check.

## Mode solvers for Slab and Circ

{py:func}`~camfr.set_solver` chooses how the modes of `Slab` and `Circ`
waveguides are found:

`track` (default)
: Finds the modes of the corresponding lossless structure, then tracks them
  as the losses and the PML are switched on. Fast and reliable for dielectric
  waveguides.

`series`
: Estimates the modes from a plane-wave (Fourier) expansion, then refines each
  with the exact dispersion relation. Suits lossy and metallic structures. The
  number of plane waves is `N × set_mode_surplus` (default 1.2).

`ADR`
: Finds the modes as the zeros of the dispersion relation inside a contour in
  the complex plane (argument principle), independent of the lossless
  structure.

`ASR`, `stretched_ASR`
: Series expansion with adaptive spatial resolution (optionally with
  stretched coordinates); `set_eta_ASR` sets the stretching. Implemented for
  TM only: for TE they fall back to `track`.

The mode search can be tuned with {py:func}`~camfr.set_precision` (guided
modes) and {py:func}`~camfr.set_precision_rad` (radiation modes): a finer scan
misses fewer modes. {py:func}`~camfr.set_degenerate`,
{py:func}`~camfr.set_chunk_tracing` and {py:func}`~camfr.set_orthogonal` help
with degenerate or badly separated modes.

### Metallic structures

In metal-clad structures light is often guided in the **low-index** regions,
for example air gaps between metal layers. The default slab solver looks for
modes guided by high-index cores and can miss these. Call
{py:func}`~camfr.set_low_index_core` `(True)` for such structures (or use the
`series` solver); see
[issue #2](https://github.com/abdelix/camfr3/issues/2).

## The Section solver

A {py:class}`~camfr.Section` (2D cross-section) is solved in two stages:

1. **Estimation**: a plane-wave expansion with `M1` terms gives estimates of
   the effective indices. This stage takes most of the time.
2. **Refinement**: each estimate is refined with a dispersion relation built
   from `M2` modes of each slab.

`Section(expression, M1, M2)` sets both; the defaults are `N × mode_surplus`
and `N`. {py:func}`~camfr.set_section_solver` selects the estimation algorithm:
`L` (default, Li's Fourier factorisation), `L_anis` (anisotropic materials),
`NT`, `OS`, `ASR_2D` and `ASR_2D_stretched`.
{py:func}`~camfr.set_mode_correction` chooses which estimates are refined
(`full` refines all modes).

**Speed.** If the effective indices are roughly known (from an earlier run, a
slab approximation or another solver), give them with
`section.set_estimate(n_eff)`, one per wanted mode: the estimation stage is
skipped, which is often 40 times faster for the same result.
`set_calc_field_profiles(False)` skips the field profiles when only the
effective indices are needed.

## Stability of stacks

Scattering matrices are computed with the S-matrix scheme, which is stable for
thick structures. Near-singular interfaces can be handled with
{py:func}`~camfr.set_stability` (`extra` equilibration or `SVD`), and
{py:func}`~camfr.set_unstable_exp_threshold` controls when growing
exponentials are dropped.

## Known problems

Open solver issues are tracked on GitHub with the
[`solver` label](https://github.com/abdelix/camfr3/issues?q=is%3Aissue+is%3Aopen+label%3Asolver),
among them:

- metal/air splitters with unphysical reflection (#1),
- `slab_no_wall` and transparent boundary conditions failing in the slab
  solver (#5),
- `Circ` with the `ADR` solver and a PML hanging (#4).

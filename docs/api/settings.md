# Global settings

These functions change CAMFR's global state; see
[Solvers and settings](../solvers.md#global-state).

## Basics

```{eval-rst}
.. autofunction:: camfr.set_lambda

.. autofunction:: camfr.get_lambda

.. autofunction:: camfr.set_N

.. autofunction:: camfr.N

.. autofunction:: camfr.set_polarisation

.. autofunction:: camfr.get_polarisation

.. autofunction:: camfr.free_tmps

.. autofunction:: camfr.free_tmp_interfaces
```

## Walls and PML

```{eval-rst}
.. autofunction:: camfr.set_lower_wall

.. autofunction:: camfr.set_upper_wall

.. autofunction:: camfr.set_left_wall

.. autofunction:: camfr.set_right_wall

.. autofunction:: camfr.set_lower_PML

.. autofunction:: camfr.set_upper_PML

.. autofunction:: camfr.set_left_PML

.. autofunction:: camfr.set_right_PML

.. autofunction:: camfr.set_circ_PML
```

## Slab and Circ mode solvers

```{eval-rst}
.. autofunction:: camfr.set_solver

.. autofunction:: camfr.set_mode_surplus

.. autofunction:: camfr.set_low_index_core

.. autofunction:: camfr.set_precision

.. autofunction:: camfr.set_precision_rad

.. autofunction:: camfr.set_precision_enhancement

.. autofunction:: camfr.set_dx_enhanced

.. autofunction:: camfr.set_degenerate

.. autofunction:: camfr.set_orthogonal

.. autofunction:: camfr.set_chunk_tracing

.. autofunction:: camfr.set_sweep_from_previous

.. autofunction:: camfr.set_sweep_steps

.. autofunction:: camfr.set_eps_trace_coarse

.. autofunction:: camfr.set_C_upperright

.. autofunction:: camfr.set_estimate_cutoff

.. autofunction:: camfr.set_eta_ASR

.. autofunction:: camfr.set_beta

.. autofunction:: camfr.set_backward_modes

.. autofunction:: camfr.set_circ_order

.. autofunction:: camfr.set_circ_field_type

.. autofunction:: camfr.set_mueller_precision
```

## Section solver

```{eval-rst}
.. autofunction:: camfr.set_section_solver

.. autofunction:: camfr.set_mode_correction

.. autofunction:: camfr.set_estimate_cutoff_section

.. autofunction:: camfr.set_keep_all_estimates

.. autofunction:: camfr.set_keep_all_1D_estimates

.. autofunction:: camfr.set_section_reduction

.. autofunction:: camfr.set_calc_field_profiles

.. autofunction:: camfr.print_estimates

.. autofunction:: camfr.set_section_eta_ASR

.. autofunction:: camfr.set_n_eff_max

.. autofunction:: camfr.set_NOV

.. autofunction:: camfr.set_u_step

.. autofunction:: camfr.set_v_step

.. autofunction:: camfr.set_percentage_stretched

.. autofunction:: camfr.set_extended_output

.. autofunction:: camfr.A_switch

.. autofunction:: camfr.B_switch

.. autofunction:: camfr.C_switch

.. autofunction:: camfr.D_switch
```

## BlochSection

```{eval-rst}
.. autofunction:: camfr.set_fourier_orders

.. autofunction:: camfr.get_fourier_orders_x

.. autofunction:: camfr.get_fourier_orders_y
```

## Stacks and cavities

```{eval-rst}
.. autofunction:: camfr.set_stability

.. autofunction:: camfr.set_unstable_exp_threshold

.. autofunction:: camfr.set_field_calc_heuristic

.. autofunction:: camfr.set_bloch_calc

.. autofunction:: camfr.set_eigen_calc

.. autofunction:: camfr.set_gain_material

.. autofunction:: camfr.set_always_recalculate

.. autofunction:: camfr.set_always_dense

.. autofunction:: camfr.set_davy
```

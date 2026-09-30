# API reference

Everything is available from the `camfr` package (`camfr.Slab`,
`camfr.set_N`, …). The solver itself is the compiled extension
`camfr._camfr`; the geometry and plotting helpers are written in Python.

Indices of modes start at 0; complex numbers follow the engineering
convention, with loss as a negative imaginary part of the refractive index.

```{toctree}
:maxdepth: 2

settings
materials
waveguides
structures
fields
geometry
plotting
enums
internals
```

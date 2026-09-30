
/////////////////////////////////////////////////////////////////////////////
//
// File:          camfr_wrap.cpp
// Author:        Peter.Bienstman@UGent.be
// Date:          20021119
// Version:       2.1
//
// Copyright (C) 2002 Peter Bienstman - Ghent University
//
/////////////////////////////////////////////////////////////////////////////

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "camfr_wrap.h"

#include "defs.h"
#include "coord.h"
#include "mode.h"
#include "field.h"
#include "material.h"
#include "waveguide.h"
#include "scatterer.h" 
#include "expression.h"
#include "stack.h"
#include "cavity.h"
#include "bloch.h"
#include "icache.h"
#include "infstack.h"
#include "primitives/planar/planar.h"
#include "primitives/circ/circ.h"
#include "primitives/slab/generalslab.h"
#include "primitives/slab/isoslab/slab.h"
#include "primitives/slab/isoslab/slabwall.h"
#include "primitives/slab/isoslab/slabdisp.h"
#include "primitives/section/section.h"
#include "primitives/section/sectiondisp.h"
#include "primitives/section/refsection.h"
#include "primitives/blochsection/blochsection.h"
#include "primitives/blochsection/blochsectionmode.h"
#include "math/calculus/polyroot/polyroot.h"
#include "math/calculus/quadrature/patterson.h"
#include "math/calculus/croot/patterson_z_n.h"

/////////////////////////////////////////////////////////////////////////////
//
// Python wrappers for the CAMFR classes.
// The wrappers are created with pybind11 (Boost.Python until 2026).
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Additional functions used in the Python interface.
//
/////////////////////////////////////////////////////////////////////////////

inline void set_lambda(Complex l)
  {global.lambda = l;}

inline Complex get_lambda()
  {return global.lambda;}

inline void set_N(int n)
  {global.N = n;}

inline int get_N()
  {return global.N;}

inline void set_polarisation(Polarisation pol)
  {global.polarisation = pol;}

inline Polarisation get_polarisation()
  {return global.polarisation;}

inline void set_gain_material(Material* m)
  {global.gain_mat = m;} 

inline void set_solver(Solver s)
  {global.solver = s;} 

inline void set_stability(Stability s)
  {global.stability = s;}

inline void set_precision(unsigned int i)
  {global.precision = i;} 

inline void set_precision_enhancement(unsigned int i)
  {global.precision_enhancement = i;}

inline void set_dx_enhanced(Real d)
  {global.dx_enhanced = d;} 

inline void set_precision_rad(unsigned int i)
  {global.precision_rad = i;} 

inline void set_C_upperright(const Complex& c)
  {global.C_upperright = c;} 

inline void set_sweep_from_previous(bool b)
  {global.sweep_from_previous = b;} 

inline void set_sweep_steps(unsigned int i)
  {global.sweep_steps = i;} 

inline void set_eps_trace_coarse(Real d)
  {global.eps_trace_coarse = d;} 

inline void set_chunk_tracing(bool b)
  {global.chunk_tracing = b;} 

inline void set_unstable_exp_threshold(Real d)
  {global.unstable_exp_threshold = d;} 

inline void set_field_calc_heuristic(Field_calc_heuristic f)
  {global.field_calc_heuristic = f;} 

inline void set_bloch_calc(Bloch_calc s)
  {global.bloch_calc = s;}

inline void set_eigen_calc(Eigen_calc s)
  {global.eigen_calc = s;}

inline void set_orthogonal(bool b)
  {global.orthogonal = b;}

inline void set_degenerate(bool b)
  {global.degenerate = b;}

inline void set_circ_order(int n)
  {global_circ.order = n;}

inline void set_circ_fieldtype(Fieldtype f)
  {global_circ.fieldtype = f;}

inline void set_lower_PML(Real PML)
{
  if (PML > 0)
    py_print("Warning: gain in PML.");

  global_slab.lower_PML = PML;
}

inline void set_upper_PML(Real PML)
{
  if (PML > 0)
    py_print("Warning: gain in PML.");

  global_slab.upper_PML = PML;
}

inline void set_left_PML(Real PML)
{
  if (PML > 0)
    py_print("Warning: gain in PML.");

  global_section.left_PML = PML;
}

inline void set_right_PML(Real PML)
{
  if (PML > 0)
    py_print("Warning: gain in PML.");

  global_section.right_PML = PML;
}

inline void set_circ_PML(Real PML)
{
  if (PML > 0)
    py_print("Warning: gain in PML.");

  global_circ.PML = PML;
}

inline void set_left_wall(Section_wall_type w)
  {global_section.leftwall = w;}

inline void set_right_wall(Section_wall_type w)
  {global_section.rightwall = w;}

inline void set_lower_wall(SlabWall* w)
  {global_slab.lowerwall = w;}

inline void set_upper_wall(SlabWall* w)
  {global_slab.upperwall = w;}

inline void set_beta(const Complex& beta)
  {global.slab_ky = beta;}

inline void set_mode_surplus(Real l)
  {global.mode_surplus = l;}

inline void set_backward_modes(bool b)
  {global.backward_modes = b;}

inline void set_keep_all_1D_estimates(bool b)
  {global.keep_all_1D_estimates=b;}

inline void set_section_solver(Section_solver s)
  {global_section.section_solver = s;}

inline void A_switch(bool b)
  {global_section.A_switch = b;}

inline void B_switch(bool b)
  {global_section.B_switch = b;}

inline void C_switch(bool b)
  {global_section.C_switch = b;}

inline void D_switch(bool b)
  {global_section.D_switch = b;}

inline void print_estimates(bool b)
  {global_section.print_estimates = b;}

inline void set_u_step(Real u_step)
  {global_section.u_step_given = u_step;}

inline void set_v_step(Real v_step)
  {global_section.v_step_given = v_step;}

inline void set_percentage_stretched(Real percentage)
  {global_section.percentage_stretched = percentage;}

inline void set_extended_output(bool output)
  {global_section.extended_output = output;}

inline void set_mode_correction(Mode_correction c)
  {global_section.mode_correction = c;}

inline void set_keep_all_estimates(bool b)
  {global_section.keep_all_estimates = b;}

inline void set_section_eta_ASR(Real eta)
{
  if ((eta > 1.0) || (eta < 0.0))
    py_print("Warning: eta_ASR should be between 0 and 1.");

  global_section.eta_ASR = eta;
} 

inline void set_eta_ASR(Real eta)
{
  if ((eta > 1.0) || (eta < 0.0))
    py_print("Warning: eta_ASR should be between 0 and 1.");

  global_slab.eta_ASR=eta;
}
 
inline void set_section_reduction(bool b)
  {global_section.reduced_eigenmatrix = b;}

inline void set_n_eff_max(Real max)
  {global_section.n_eff_max = max;}

inline void set_NOV(int nov)
  {global_section.number_of_values=nov;}

inline void set_estimate_cutoff(Real d)
  {global_slab.estimate_cutoff = d;}

inline void set_estimate_cutoff_section(Real d)
  {global_section.estimate_cutoff = d;}

inline void set_low_index_core(bool b)
  {global_slab.low_index_core = b;}

inline void set_davy(bool b)
  {global.davy = b;}

inline void set_always_recalculate(bool b)
  {global.always_recalculate = b;}

inline void set_calc_field_profiles(bool b)
  {global.calc_field_profiles = b;}

inline void set_always_dense(bool b)
  {global.always_dense = b;}

inline void set_mueller_precision(Real d)
  {global.mueller_precision = d;}

inline void set_fourier_orders(int Mx, int My=0)
{
  global_blochsection.Mx = Mx;
  global_blochsection.My = My;
  global.N = 2*(2*Mx+1)*(2*My+1);
}

inline int get_fourier_orders_x()
   {return global_blochsection.Mx;}

inline int get_fourier_orders_y()
   {return global_blochsection.My;}

inline Polarisation mode_pol(const Mode& m) {return m.pol;}

inline Complex field_E1(const Field& f) {return f.E1;}
inline Complex field_E2(const Field& f) {return f.E2;}
inline Complex field_Ez(const Field& f) {return f.Ez;}
inline Complex field_H1(const Field& f) {return f.H1;}
inline Complex field_H2(const Field& f) {return f.H2;}
inline Complex field_Hz(const Field& f) {return f.Hz;}

inline Mode* waveguide_get_mode(const Waveguide& w, int i)
  {check_wg_index(w,i); return w.get_mode(i+1);}
inline Mode* waveguide_get_fw_mode(const Waveguide& w, int i)
  {check_wg_index(w,i); return w.get_fw_mode(i+1);}
inline Mode* waveguide_get_bw_mode(const Waveguide& w, int i)
  {check_wg_index(w,i); return w.get_bw_mode(i+1);}

inline Complex stack_R12(const Stack& s, int i, int j)
  {check_index(i); check_index(j); return s.R12(i+1,j+1);}
inline Complex stack_R21(const Stack& s, int i, int j)
  {check_index(i); check_index(j); return s.R21(i+1,j+1);}
inline Complex stack_T12(const Stack& s, int i, int j)
  {check_index(i); check_index(j); return s.T12(i+1,j+1);}
inline Complex stack_T21(const Stack& s, int i, int j)
  {check_index(i); check_index(j); return s.T21(i+1,j+1);}

inline Real stack_inc_S_flux(Stack& s, Real c1_start, Real c1_stop, Real eps)
  {return dynamic_cast<MultiWaveguide*>(s.get_inc())
     ->S_flux(s.inc_field_expansion(),c1_start,c1_stop,eps);}

inline Real stack_ext_S_flux(Stack& s, Real c1_start, Real c1_stop, Real eps)
  {return dynamic_cast<MultiWaveguide*>(s.get_ext())
     ->S_flux(s.ext_field_expansion(),c1_start,c1_stop,eps);}

inline py::tuple stack_fw_bw(Stack& s, Real z)
{
  cVector fw(global.N,fortranArray);
  cVector bw(global.N,fortranArray);

  s.fw_bw_field(Coord(0,0,z), &fw, &bw);

  return py::make_tuple(fw, bw);
}

inline py::tuple stack_fw_bw_2(Stack& s, Real z, Limit l)
{
  cVector fw(global.N,fortranArray);
  cVector bw(global.N,fortranArray);

  s.fw_bw_field(Coord(0,0,z,Plus,Plus,l), &fw, &bw);

  return py::make_tuple(fw, bw);
}

inline Real stack_length(Stack& s) 
  {return real(s.get_total_thickness());}
inline Real stack_width(Stack& s) 
  {return real(s.get_inc()->c1_size());}

inline void free_tmp_interfaces(Waveguide& w)
  {interface_cache.deregister(&w);}



/////////////////////////////////////////////////////////////////////////////
//
// Wrapper functions warning about deprecated features.
//
/////////////////////////////////////////////////////////////////////////////

Term material_to_term(BaseMaterial& m, const Complex& d)
{
  if (real(d) < 0)
    py_print("Warning: negative real length of material.");

  if(abs(imag(d)) > 0)
    py_error("Error: complex thickness deprecated in CAMFR 1.0.");
  
  return Term(m(d));
} 

Term waveguide_to_term(Waveguide& w, const Complex& d)
{
  if (real(d) < 0)
    py_print("Warning: negative real length of waveguide.");

  if(abs(imag(d)) > 0)
    py_error("Error: complex thickness deprecated in CAMFR 1.0.");
  
  return Term(w(d));
}



/////////////////////////////////////////////////////////////////////////////
//
// The following functions are used when expanding an abritrarily shaped 
// field in slabmodes.
//
/////////////////////////////////////////////////////////////////////////////

inline void stack_set_inc_field_function(Stack& s, py::object o, Real eps)
{
  Slab* slab = dynamic_cast<Slab*>(s.get_inc());
  
  if (!slab)
  {
    py_error("set_inc_field_function only implemented for slabs.");
    exit(-1);
  }

  PythonFunction f(o);
  s.set_inc_field(slab->expand_field(&f, eps));
}

inline void stack_set_inc_field_gaussian
  (Stack& s, Complex height, Complex width, Complex pos, Real eps)
{
  Slab* slab = dynamic_cast<Slab*>(s.get_inc());
  
  if (!slab)
  {
    py_error("set_inc_field_gaussian only implemented for slabs.");
    exit(-1);
  }

  GaussianFunction f(height,width,pos);
  s.set_inc_field(slab->expand_field(&f, eps));
}

inline void stack_set_inc_field_plane_wave
  (Stack& s, const Complex& amplitude, const Complex& angle, Real eps)
{
  Slab* slab = dynamic_cast<Slab*>(s.get_inc());
 
  if (!slab)
  {
    py_error("set_inc_field_plane_Wave only implemented for slabs.");
    exit (-1);
  }

  Complex index = slab->get_core()->n();
  PlaneWaveFunction f(amplitude,angle,index);
  s.set_inc_field(slab->expand_field(&f, eps));
}


  
/////////////////////////////////////////////////////////////////////////////
//
// Functions dealing with handling of default arguments.
//
/////////////////////////////////////////////////////////////////////////////

Complex stack_lateral_S_flux(Stack& s, Real c)
  {return s.lateral_S_flux(c);}

Complex stack_lateral_S_flux_2(Stack& s, Real c, int k)
  {std::vector<Complex> S_k; s.lateral_S_flux(c, &S_k); return S_k[k];}

void stack_set_inc_field(Stack& s, const cVector& f)
  {s.set_inc_field(f);}

void stack_set_inc_field_2(Stack& s, const cVector& f, const cVector& b) 
  {s.set_inc_field(f, &const_cast<cVector&>(b));}

Complex material_epsr(Material& m)
  {return m.epsr();}

Complex basematerial_epsr(Material& m, int i)
  {return m.epsr(i);}

Complex material_mur(Material& m)
  {return m.mur();}

Complex basematerial_mur(Material& m, int i)
  {return m.mur(i);}



/////////////////////////////////////////////////////////////////////////////
//
// Enums.
//
//   The values are also exported at module level (e.g. camfr.TE). As with
//   Boost.Python, str() gives the bare name ('TE').
//
/////////////////////////////////////////////////////////////////////////////

// Assigned rather than added with .def(): .def() would only append an overload
// to the enum's own __str__, which matches first (pybind11 3.0).

template <class E>
void name_as_str(py::enum_<E>& e)
{
  py::setattr(e, "__str__",
              py::cpp_function([](py::handle h) {return py::str(h.attr("name"));},
                               py::name("__str__"), py::is_method(e)));
}



/////////////////////////////////////////////////////////////////////////////
//
// The CAMFR module itself
//
/////////////////////////////////////////////////////////////////////////////

void camfr_wrap_2(py::module_& m);

PYBIND11_MODULE(_camfr, m)
{
  // Object lifetimes. The C++ objects keep raw pointers to the objects they
  // are built from (a Term to its Material or Waveguide, an Expression to its
  // Terms, a Stack or Slab to its Expression, ...). keep_alive ties the
  // lifetime of those Python objects to the new one, so that e.g.
  // Stack(wg(0) + Slab(air(2))(0)) no longer leaves a dangling pointer to the
  // temporary Slab. Setters that store a pointer in a global keep the object
  // in a module attribute.

  // Docstrings follow the Google style (Sphinx napoleon); pybind11 adds the
  // signature line itself.

  m.doc() = "CAMFR extension module: the eigenmode-expansion solver itself.";

  py::handle mh = m;

  // Wrap Limit enum.

  py::enum_<Limit> limit(m, "Limit", py::arithmetic(),
    "Side of a discontinuity on which a field is evaluated (see Coord).");
  limit
    .value("Plus", Plus, "Just after the coordinate (the + side).")
    .value("Min",  Min,  "Just before the coordinate (the - side).")
    .export_values();
  name_as_str(limit);

  // Wrap Solver enum.

  py::enum_<Solver> solver(m, "Solver", py::arithmetic(),
    "Root finder for the modes of Slab and Circ waveguides (set_solver).");
  solver
    .value("ADR",           ADR,
           "Contour integration (argument principle) in the complex plane.")
    .value("track",         track,
           "Root tracking from a lossless structure (default).")
    .value("series",        series,
           "Plane-wave (series) estimates refined with the full dispersion "
           "relation; good for lossy and metallic slabs.")
    .value("ASR",           ASR,
           "Adaptive spatial resolution series expansion.")
    .value("stretched_ASR", stretched_ASR,
           "ASR with a stretched coordinate transformation.")
    .export_values();
  name_as_str(solver);

  // Wrap Stability enum.

  py::enum_<Stability> stability(m, "Stability", py::arithmetic(),
    "Treatment of nearly singular matrices (set_stability).");
  stability
    .value("normal", normal, "No special measures (default).")
    .value("extra",  extra,  "Row and column equilibration.")
    .value("SVD",    SVD,    "Pseudo-inverse from a singular value "
                             "decomposition.")
    .export_values();
  name_as_str(stability);

  // Wrap Field_calc_heuristic enum.

  py::enum_<Field_calc_heuristic> heuristic
    (m, "Field_calc_heuristic", py::arithmetic(),
     "Excitation used to compute fields inside stacks "
     "(set_field_calc_heuristic).");
  heuristic
    .value("identical", identical, "S-type excitation (default).")
    .value("symmetric", symmetric, "T-type excitation.")
    .export_values();
  name_as_str(heuristic);

  // Wrap Bloch_calc enum.

  py::enum_<Bloch_calc> bloch_calc(m, "Bloch_calc", py::arithmetic(),
    "Algorithm for the Bloch modes of a BlochStack (set_bloch_calc).");
  bloch_calc
    .value("GEV", GEV, "Generalised eigenvalue problem (default).")
    .value("T",   T,   "Eigenvalues of the transfer matrix.")
    .export_values();
  name_as_str(bloch_calc);

  // Wrap Eigen_calc enum.

  py::enum_<Eigen_calc> eigen_calc(m, "Eigen_calc", py::arithmetic(),
    "Algorithm for the lowest eigenvalue in cavity calculations "
    "(set_eigen_calc).");
  eigen_calc
    .value("lapack",  lapack,  "Full LAPACK eigenvalue solve (default).")
    .value("arnoldi", arnoldi, "Arnoldi iteration.")
    .export_values();
  name_as_str(eigen_calc);

  // Wrap Polarisation enum.

  py::enum_<Polarisation> polarisation(m, "Polarisation", py::arithmetic(),
    "Polarisation of a mode, or the one selected with set_polarisation.");
  polarisation
    .value("unknown", unknown, "Not determined.")
    .value("TEM",     TEM,     "Transverse electromagnetic.")
    .value("TE",      TE,      "Transverse electric (default).")
    .value("TM",      TM,      "Transverse magnetic.")
    .value("HE",      HE,      "Hybrid mode, HE type (circular waveguides).")
    .value("EH",      EH,      "Hybrid mode, EH type (circular waveguides).")
    .value("TE_TM",   TE_TM,   "Coupled TE and TM (hybrid) modes.")
    .export_values();
  name_as_str(polarisation);

  // Wrap Fieldtype enum.

  py::enum_<Fieldtype> fieldtype(m, "Fieldtype", py::arithmetic(),
    "Angular dependence of the field of a source in a Circ "
    "(set_circ_field_type).");
  fieldtype
    .value("cos_type", cos_type, "cos(order*phi) dependence (default).")
    .value("sin_type", sin_type, "sin(order*phi) dependence.")
    .export_values();
  name_as_str(fieldtype);

  // Wrap Section_wall_type enum.

  py::enum_<Section_wall_type> wall_type
    (m, "Section_wall_type", py::arithmetic(),
     "Lateral boundary of a Section (set_left_wall, set_right_wall).");
  wall_type
    .value("E_wall",  E_wall,  "Electric wall (default).")
    .value("H_wall",  H_wall,  "Magnetic wall.")
    .value("no_wall", no_wall, "Open boundary.")
    .export_values();
  name_as_str(wall_type);

  // Wrap Sort_type.

  py::enum_<Sort_type> sort_type(m, "Sort_type", py::arithmetic(),
    "Mode order in a Section (Section.set_sorting).");
  sort_type
    .value("highest_index", highest_index, "Highest effective index first.")
    .value("lowest_loss",   lowest_loss,   "Lowest loss first.")
    .export_values();
  name_as_str(sort_type);

  // Wrap Section_solver enum.

  py::enum_<Section_solver> section_solver
    (m, "Section_solver", py::arithmetic(),
     "Algorithm that estimates the modes of a Section before refinement "
     "(set_section_solver).");
  section_solver
    .value("OS",               OS,
           "Omar-Schuenemann estimates.")
    .value("NT",               NT,
           "Plane-wave (Fourier) estimates, older variant.")
    .value("L",                L,
           "Plane-wave (Fourier) estimates, Li's factorisation (default).")
    .value("L_anis",           L_anis,
           "As L, for anisotropic materials.")
    .value("ASR_2D",           ASR_2D,
           "Adaptive spatial resolution.")
    .value("ASR_2D_stretched", ASR_2D_stretched,
           "Adaptive spatial resolution with coordinate stretching.")
    .export_values();
  name_as_str(section_solver);

  // Wrap Mode_correction enum.

  py::enum_<Mode_correction> mode_correction
    (m, "Mode_correction", py::arithmetic(),
     "Refinement of Section mode estimates (set_mode_correction).");
  mode_correction
    .value("none",        none,        "Keep the estimates (default).")
    .value("snap",        snap,        "Snap estimates to nearby roots.")
    .value("guided_only", guided_only, "Refine the guided modes only.")
    .value("full",        full,        "Refine all modes.")
    .export_values();
  name_as_str(mode_correction);

  // Wrap getters and setters for global parameters.

  m.def("set_lambda",                 set_lambda, py::arg("wavelength"),
        "Set the wavelength. All lengths are in the same unit, normally "
        "micrometre (Material.gain assumes it).");
  m.def("get_lambda",                 get_lambda,
        "Return the wavelength. (``lambda`` is a Python keyword, hence the "
        "``get_`` prefix.)");
  m.def("set_N",                      set_N, py::arg("N"),
        "Set the number of modes used in the eigenmode expansion.");
  m.def("N",                          get_N,
        "Return the number of modes used in the eigenmode expansion.");
  m.def("set_polarisation",           set_polarisation, py::arg("pol"),
        R"doc(Set the polarisation: TE (default) or TM.

Only used where the polarisations decouple: 2D Cartesian structures (Slab,
Planar) and Circ with Bessel order 0.)doc");
  m.def("get_polarisation",           get_polarisation,
        "Return the polarisation set with set_polarisation.");
  m.def("set_gain_material",          [mh](py::object mat)
        {mh.attr("_gain_material") = mat;
         set_gain_material(mat.cast<Material*>());}, py::arg("material"),
        R"doc(Set the gain material of a Cavity.

The imaginary part of this material's index is varied to find a lasing mode
(Cavity.find_mode).)doc");
  m.def("set_solver",                 set_solver, py::arg("solver"),
        R"doc(Select the root finder for the modes of Slab and Circ waveguides.

Args:
    solver: ``track`` (default), ``ADR``, ``series``, ``ASR`` or
        ``stretched_ASR``. ``series`` first estimates the modes with a
        plane-wave expansion (``set_mode_surplus`` sets its size) and then
        refines them; it suits lossy and metallic structures.)doc");
  m.def("set_stability",              set_stability, py::arg("stability"),
        "Set the treatment of nearly singular matrices: normal (default), "
        "extra or SVD.");
  m.def("set_precision",              set_precision, py::arg("precision"),
        "Set the resolution of the scan for guided modes (default 100; higher "
        "misses fewer modes but is slower).");
  m.def("set_precision_enhancement",  set_precision_enhancement,
        py::arg("factor"),
        "If > 1 (default 1), rescan around each guided mode with "
        "precision*factor, to separate nearly degenerate modes.");
  m.def("set_dx_enhanced",            set_dx_enhanced, py::arg("dx"),
        "Relative half-width of the rescan region of "
        "set_precision_enhancement (default 0.01).");
  m.def("set_precision_rad",          set_precision_rad, py::arg("precision"),
        "Set the resolution of the scan for radiation modes (default 100).");
  m.def("set_C_upperright",           set_C_upperright, py::arg("factor"),
        "Scale the upper right corner of the complex region searched for "
        "complex modes, relative to the default (default 1+1j).");
  m.def("set_sweep_from_previous",    set_sweep_from_previous, py::arg("b"),
        "Start the mode search from the modes of the previous calculation "
        "(e.g. a nearby wavelength). Faster, sometimes less stable. Default "
        "False.");
  m.def("set_sweep_steps",            set_sweep_steps, py::arg("steps"),
        "Initial number of steps of the tracking root finder (default 20).");
  m.def("set_eps_trace_coarse",       set_eps_trace_coarse, py::arg("eps"),
        "Coarse intermediate precision of the tracking root finder "
        "(default 1e-14).");
  m.def("set_chunk_tracing",          set_chunk_tracing, py::arg("b"),
        "Track the modes in chunks rather than all together. Default True: "
        "faster, but can lose modes, especially with strong PML absorption.");
  m.def("set_unstable_exp_threshold", set_unstable_exp_threshold,
        py::arg("eps"),
        "Growing exponentials smaller than this are set to zero (default "
        "1e-12). Larger values (e.g. 1e-6) decouple waveguides sooner.");
  m.def("set_field_calc_heuristic",   set_field_calc_heuristic,
        py::arg("heuristic"),
        "Select the excitation used to compute fields in stacks: identical "
        "(default) or symmetric.");
  m.def("set_bloch_calc",             set_bloch_calc, py::arg("method"),
        "Select the Bloch mode algorithm: GEV (default) or T.");
  m.def("set_eigen_calc",             set_eigen_calc, py::arg("method"),
        "Select the cavity eigenvalue algorithm: lapack (default) or "
        "arnoldi.");
  m.def("set_orthogonal",             set_orthogonal, py::arg("b"),
        "Treat the modes as orthogonal (default True). False can improve "
        "convergence when rounding errors spoil their orthogonality.");
  m.def("set_degenerate",             set_degenerate, py::arg("b"),
        "Take special care to find degenerate modes (default True).");
  m.def("set_circ_order",             set_circ_order, py::arg("order"),
        "Set the order of the Bessel modes (and angular dependence) in Circ "
        "waveguides (default 1).");
  m.def("set_circ_field_type",        set_circ_fieldtype, py::arg("fieldtype"),
        "Set the angular field dependence of sources in Circ waveguides: "
        "cos_type (default) or sin_type.");
  m.def("set_left_wall",              set_left_wall, py::arg("wall"),
        "Set the left boundary of subsequently defined Sections: E_wall "
        "(default), H_wall or no_wall.");
  m.def("set_right_wall",             set_right_wall, py::arg("wall"),
        "Set the right boundary of subsequently defined Sections: E_wall "
        "(default), H_wall or no_wall.");
  m.def("set_upper_wall",             [mh](py::object w)
        {mh.attr("_upper_wall") = w; set_upper_wall(w.cast<SlabWall*>());},
        py::arg("wall"),
        "Set the wall at x = width of subsequently defined Slabs (e.g. "
        "slab_E_wall, slab_H_wall). Default: electric wall. Slab."
        "set_upper_wall sets it for one slab.");
  m.def("set_lower_wall",             [mh](py::object w)
        {mh.attr("_lower_wall") = w; set_lower_wall(w.cast<SlabWall*>());},
        py::arg("wall"),
        "Set the wall at x = 0 of subsequently defined Slabs (e.g. "
        "slab_E_wall, slab_H_wall). Default: electric wall. Slab."
        "set_lower_wall sets it for one slab.");
  m.def("set_left_PML",               set_left_PML, py::arg("PML"),
        "Give the left cladding of subsequently defined Sections an "
        "imaginary thickness PML*1j (PML < 0 absorbs). Default 0.");
  m.def("set_right_PML",              set_right_PML, py::arg("PML"),
        "Give the right cladding of subsequently defined Sections an "
        "imaginary thickness PML*1j (PML < 0 absorbs). Default 0.");
  m.def("set_upper_PML",              set_upper_PML, py::arg("PML"),
        R"doc(Add a PML to the upper cladding of subsequently defined Slabs.

The last layer of the slab expression (at x = width) gets an imaginary
thickness ``PML*1j``; ``PML`` is normally negative, for absorption. Default 0.
Set it before the structures are defined.)doc");
  m.def("set_lower_PML",              set_lower_PML, py::arg("PML"),
        R"doc(Add a PML to the lower cladding of subsequently defined Slabs.

The first layer of the slab expression (at x = 0) gets an imaginary thickness
``PML*1j``; ``PML`` is normally negative, for absorption. Default 0. Set it
before the structures are defined.)doc");
  m.def("set_circ_PML",               set_circ_PML, py::arg("PML"),
        "Give the cladding of subsequently defined Circ waveguides an "
        "imaginary thickness PML*1j (PML < 0 absorbs). Default 0.");
  m.def("set_eta_ASR",                set_eta_ASR, py::arg("eta"),
        "Stretching parameter (0 to 1) of the ASR slab solver (default 1).");
  m.def("set_section_reduction",      set_section_reduction, py::arg("b"),
        "Use the reduced eigenmatrix in the Section solver (default True).");
  m.def("set_n_eff_max",              set_n_eff_max, py::arg("n_eff"),
        "ASR_2D section solver: discard estimates with a larger effective "
        "index (default 10).");
  m.def("set_NOV",                    set_NOV, py::arg("nov"),
        "ASR_2D section solver: number of eigenvalues kept (default 50).");
  m.def("set_estimate_cutoff",        set_estimate_cutoff, py::arg("factor"),
        "Slab solver: safety factor for the range of the mode search "
        "(default 1.2).");
  m.def("set_estimate_cutoff_section",set_estimate_cutoff_section,
        py::arg("factor"),
        "Section solver: discard plane-wave estimates with kz above factor "
        "times the largest material wavenumber (default 2).");
  m.def("set_low_index_core",         set_low_index_core, py::arg("b"),
        R"doc(Search for modes guided in low-index regions (default False).

Needed for metallic structures whose light travels in low-index gaps, e.g.
air between metal layers: with the default, the slab solver can miss those
modes.)doc");
  m.def("set_beta",                   set_beta, py::arg("beta"),
        "Set the out-of-plane wavenumber beta (along y) of Slabs, for "
        "off-plane incidence. Default 0.");
  m.def("set_section_solver",         set_section_solver, py::arg("solver"),
        "Select the Section estimation algorithm: L (default), L_anis, NT, "
        "OS, ASR_2D or ASR_2D_stretched.");
  m.def("set_section_eta_ASR",        set_section_eta_ASR, py::arg("eta"),
        "Stretching parameter (0 to 1) of the ASR_2D section solver "
        "(default 1).");
  m.def("A_switch",                   A_switch, py::arg("b"),
        "Expert switch of the ASR_2D section solver (default False).");
  m.def("B_switch",                   B_switch, py::arg("b"),
        "Expert switch of the ASR_2D section solver (default True).");
  m.def("C_switch",                   C_switch, py::arg("b"),
        "Expert switch of the ASR_2D section solver (default False).");
  m.def("D_switch",                   D_switch, py::arg("b"),
        "Expert switch of the ASR_2D section solver (default True).");
  m.def("print_estimates",            print_estimates, py::arg("b"),
        "Print the Section mode estimates (default False).");
  m.def("set_u_step",                 set_u_step, py::arg("u_step"),
        "ASR_2D_stretched section solver: stretching step in x "
        "(default 0: automatic).");
  m.def("set_v_step",                 set_v_step, py::arg("v_step"),
        "ASR_2D_stretched section solver: stretching step in y "
        "(default 0: automatic).");
  m.def("set_percentage_stretched",   set_percentage_stretched,
        py::arg("fraction"),
        "ASR_2D_stretched section solver: amount of stretching, from 0 "
        "(plain ASR) to 1 (default).");
  m.def("set_extended_output",        set_extended_output, py::arg("b"),
        "ASR_2D section solver: print diagnostic output (default False).");
  m.def("set_keep_all_estimates",     set_keep_all_estimates, py::arg("b"),
        "Section solver: keep all plane-wave estimates and set N to their "
        "number (default False).");
  m.def("set_mode_correction",        set_mode_correction,
        py::arg("correction"),
        "Select how Section mode estimates are refined: none (default), "
        "snap, guided_only or full.");
  m.def("set_mode_surplus",           set_mode_surplus, py::arg("factor"),
        "Size of the auxiliary expansion as a multiple of N (default 1.2): "
        "plane waves of the series slab solver, default M1 of a Section.");
  m.def("set_backward_modes",         set_backward_modes, py::arg("b"),
        "Extra stability for Circ structures with backward and complex "
        "modes (e.g. a metal wall close to the last interface). Slower; "
        "default False.");
  m.def("set_keep_all_1D_estimates",  set_keep_all_1D_estimates,
        py::arg("b"),
        "Keep all 1D (slab) estimates in the Section solver (default "
        "False).");
  m.def("set_fourier_orders",         set_fourier_orders,
        py::arg("Mx"), py::arg("My")=0,
        "Set the Fourier orders of BlochSection in x and y; also sets "
        "N = 2*(2*Mx+1)*(2*My+1).");
  m.def("get_fourier_orders_x",       get_fourier_orders_x,
        "Return the number of Fourier orders Mx of BlochSection.");
  m.def("get_fourier_orders_y",       get_fourier_orders_y,
        "Return the number of Fourier orders My of BlochSection.");
  m.def("set_davy",                   set_davy, py::arg("b"),
        "Debug: write the slab overlap matrices to files (default False).");
  m.def("set_always_recalculate",     set_always_recalculate, py::arg("b"),
        "Recalculate waveguides and stacks even if nothing changed "
        "(default False).");
  m.def("set_calc_field_profiles",    set_calc_field_profiles, py::arg("b"),
        "Compute the field profiles of Section modes (default True). False "
        "only gives the effective indices, faster.");
  m.def("set_always_dense",           set_always_dense, py::arg("b"),
        "Treat all interfaces as dense, also between uniform waveguides "
        "(default False).");
  m.def("set_mueller_precision",      set_mueller_precision,
        py::arg("precision"),
        "Precision of the Mueller root finder (default 1e-14).");
  m.def("free_tmps",                  free_tmps,
        R"doc(Free all cached interface scattering matrices.

Saves memory, e.g. at the end of an inner loop; the matrices are recomputed
when needed again. Call it between independent calculations.)doc");
  m.def("free_tmp_interfaces",        free_tmp_interfaces,
        py::arg("waveguide"),
        "Free the cached interface matrices that involve this waveguide.");

  // Internal: polyroot, for testsuite/polyroot.py.

  m.def("_polyroot", [](py::array_t<Complex, py::array::forcecast> c)
  {
    auto r = c.unchecked<1>();
    std::vector<Complex> coef(r.data(0), r.data(0) + r.shape(0));
    std::vector<Complex> roots = polyroot(coef);
    py::array_t<Complex> out(roots.size());
    std::copy(roots.begin(), roots.end(), out.mutable_data());
    return out;
  }, py::arg("coefficients"),
     "Internal (tests): roots of a polynomial, coefficients from the highest "
     "degree down.");

  // Internal: Patterson quadrature, for testsuite/patterson.py.

  m.def("_patterson", [](py::object f, Real a, Real b, Real eps,
                         unsigned int max_k)
  {
    struct F : public RealFunction
    {
      py::object f;
      F(py::object f_) : f(f_) {}
      Real operator()(const Real& x) {counter++; return f(x).cast<Real>();}
    } func(f);

    bool error;
    Real abs_error;
    Real result = patterson(func, a, b, eps, &error, max_k, &abs_error);
    return py::make_tuple(result, error, func.times_called());
  }, py::arg("f"), py::arg("a"), py::arg("b"), py::arg("eps"),
     py::arg("max_k")=8,
     "Internal (tests): integral of f from a to b with the Patterson "
     "formulas; returns (result, not_converged, evaluations).");

  m.def("_patterson_z_n", [](py::object f, Complex a, Complex b, int M,
                             Real eps, unsigned int max_k)
  {
    PythonFunction func(f);
    bool error;
    std::vector<Complex> r
      = patterson_z_n(func, a, b, M, eps, 1e300, &error, max_k);
    py::array_t<Complex> out(r.size());
    std::copy(r.begin(), r.end(), out.mutable_data());
    return py::make_tuple(out, error);
  }, py::arg("f"), py::arg("a"), py::arg("b"), py::arg("M"), py::arg("eps"),
     py::arg("max_k")=8,
     "Internal (tests): integrals of z**n/f(z), n = 0..M, along the segment "
     "from a to b; returns (results, not_converged).");

  // Wrap Coord.

  py::class_<Coord>(m, "Coord", R"doc(A point in space.

``c1`` and ``c2`` are x and y in Cartesian structures, rho and phi in
cylindrical ones. The optional Limits (Plus or Min) choose the side of an
index discontinuity at which a field is evaluated.)doc")
    .def(py::init<const Real&, const Real&, const Real&>(),
         py::arg("c1"), py::arg("c2"), py::arg("z"))
    .def(py::init<const Real&, const Real&, const Real&, Limit>(),
         py::arg("c1"), py::arg("c2"), py::arg("z"), py::arg("c1_limit"))
    .def(py::init<const Real&, const Real&, const Real&, Limit, Limit>(),
         py::arg("c1"), py::arg("c2"), py::arg("z"),
         py::arg("c1_limit"), py::arg("c2_limit"))
    .def(py::init<const Real&, const Real&, const Real&,
                  Limit, Limit, Limit>(),
         py::arg("c1"), py::arg("c2"), py::arg("z"),
         py::arg("c1_limit"), py::arg("c2_limit"), py::arg("z_limit"))
    .def("__repr__", &Coord::repr)
    ;

  // Wrap Field.

  py::class_<Field>(m, "Field",
    "The electromagnetic field at a point. Components 1 and 2 are x and y "
    "(Cartesian) or rho and phi (cylindrical).")
    .def("E1",       field_E1, "First transverse component of E.")
    .def("E2",       field_E2, "Second transverse component of E.")
    .def("Ez",       field_Ez, "z component of E.")
    .def("H1",       field_H1, "First transverse component of H.")
    .def("H2",       field_H2, "Second transverse component of H.")
    .def("Hz",       field_Hz, "z component of H.")
    .def("S1",       &Field::S1,
         "First transverse component of the Poynting vector E x H*.")
    .def("S2",       &Field::S2,
         "Second transverse component of the Poynting vector E x H*.")
    .def("Sz",       &Field::Sz, "z component of the Poynting vector E x H*.")
    .def("abs_E",    &Field::abs_E, "Magnitude of E.")
    .def("abs_H",    &Field::abs_H, "Magnitude of H.")
    .def("abs_S",    &Field::abs_S, "Magnitude of the Poynting vector.")
    .def("__repr__", &Field::repr)
    ;

  // Wrap FieldExpansion.

  py::class_<FieldExpansion>(m, "FieldExpansion",
    "A field given by its mode expansion in a waveguide.")
    .def("field",    &FieldExpansion::field, py::arg("coord"),
         "Return the Field at a Coord.")
    .def("__repr__", &FieldExpansion::repr)
    ;

  // Wrap BaseMaterial.

  py::class_<BaseMaterial>(m, "BaseMaterial",
    "Base class of Material and BiaxialMaterial.");

  // Wrap Material.

  py::class_<Material, BaseMaterial>(m, "Material", R"doc(An isotropic material.

``Material(n)`` has refractive index ``n``; a negative imaginary part is loss.
``Material(n, etar)`` sets eps_r = n*etar and mu_r = n/etar (``etar`` defaults
to ``n``, i.e. mu_r = 1). Calling a material with a thickness gives a Term,
``Si(0.22)``, from which Slabs and Stacks are built.)doc")
    .def(py::init<const Complex&>(), py::arg("n"))
    .def(py::init<const Complex&, const Complex&>(),
         py::arg("n"), py::arg("etar"))
    .def("__call__",     material_to_term, py::keep_alive<0, 1>(),
         py::arg("d"), "Return a layer (Term) of this material, thickness d.")
    .def("epsr",         material_epsr, "Relative permittivity.")
    .def("mur",          material_mur, "Relative permeability.")
    .def("eps",          &Material::eps, "Permittivity (F/m).")
    .def("mu",           &Material::mu, "Permeability (H/m).")
    .def("n",            &Material::n, "Refractive index.")
    .def("etar",         &Material::etar, "The etar parameter (eps_r/n).")
    .def("eta",          &Material::eta, "etar*sqrt(eps0/mu0).")
    .def("set_epsr_mur", &Material::set_epsr_mur,
         py::arg("epsr"), py::arg("mur"),
         "Set the relative permittivity and permeability.")
    .def("set_epsr",     &Material::set_epsr, py::arg("epsr"),
         "Set the relative permittivity, keeping the permeability.")
    .def("set_mur",      &Material::set_mur, py::arg("mur"),
         "Set the relative permeability, keeping the permittivity.")
    .def("set_n",        &Material::set_n, py::arg("n"),
         "Set the refractive index (etar unchanged).")
    .def("set_etar",     &Material::set_etar, py::arg("etar"),
         "Set etar (n unchanged).")
    .def("gain",         &Material::gain,
         "Material gain at the current wavelength, in 1/cm.")
    .def("__repr__",     &Material::repr)
    ;

  // Wrap BiaxialMaterial.

  py::class_<BiaxialMaterial, BaseMaterial>(m, "BiaxialMaterial",
    "A biaxial material, given by the diagonal elements of its relative "
    "permittivity and permeability tensors.")
    .def(py::init<const Complex&, const Complex&, const Complex&,
                  const Complex&, const Complex&, const Complex&>(),
         py::arg("epsr_1"), py::arg("epsr_2"), py::arg("epsr_z"),
         py::arg("mur_1"), py::arg("mur_2"), py::arg("mur_z"))
    .def("__call__", material_to_term, py::keep_alive<0, 1>(),
         py::arg("d"), "Return a layer (Term) of this material, thickness d.")
    .def("epsr",     basematerial_epsr, py::arg("i"),
         "Relative permittivity along axis i (1, 2 or 3 = z).")
    .def("mur",      basematerial_mur, py::arg("i"),
         "Relative permeability along axis i (1, 2 or 3 = z).")
    .def("__repr__", &BiaxialMaterial::repr)
    ;

  // Wrap Material_length.

  py::class_<Material_length>(m, "Material_length", "Internal.");

  // Wrap Mode.

  py::class_<Mode>(m, "Mode", "An eigenmode of a waveguide.")
    .def("field",    &Mode::field, py::arg("coord"),
         "Return the mode's Field at a Coord (z is ignored).")
    .def("n_eff",    &Mode::n_eff, "Effective index.")
    .def("kz",       &Mode::get_kz, "Propagation constant.")
    .def("pol",      mode_pol, "Polarisation.")
    .def("__repr__", &Mode::repr)
    ;

  // Wrap Waveguide.

  py::class_<Waveguide>(m, "Waveguide", R"doc(A waveguide cross-section.

Base class of Slab, Circ, Planar, Section and the other waveguides. Calling a
waveguide with a length, ``wg(d)``, gives a Term for a Stack.)doc")
    .def("core",     &Waveguide::get_core,
         py::return_value_policy::reference, "Return the core Material.")
    .def("epsr",     &Waveguide::epsr_at, py::arg("coord"),
         "Relative permittivity at a Coord.")
    .def("mur",      &Waveguide::mur_at, py::arg("coord"),
         "Relative permeability at a Coord.")
    .def("eps",      &Waveguide::eps_at, py::arg("coord"),
         "Permittivity at a Coord.")
    .def("mu",       &Waveguide::mu_at, py::arg("coord"),
         "Permeability at a Coord.")
    .def("n",        &Waveguide::n_at, py::arg("coord"),
         "Refractive index at a Coord.")
    .def("etar",     &Waveguide::etar_at, py::arg("coord"),
         "The Material etar parameter at a Coord.")
    .def("N",        &Waveguide::N,
         "Number of modes in this waveguide (usually N()).")
    .def("mode",     waveguide_get_mode,
         py::return_value_policy::reference_internal, py::arg("i"),
         "Return mode i (0 is the fundamental mode).")
    .def("fw_mode",  waveguide_get_fw_mode,
         py::return_value_policy::reference_internal, py::arg("i"),
         "Return forward mode i.")
    .def("bw_mode",  waveguide_get_bw_mode,
         py::return_value_policy::reference_internal, py::arg("i"),
         "Return backward mode i.")
    .def("calc",     &Waveguide::find_modes, "Calculate the modes.")
    .def("__repr__", &Waveguide::repr)
    .def("__call__", waveguide_to_term, py::keep_alive<0, 1>(),
         py::arg("d"), "Return a Term: this waveguide over a length d.")
    ;

  // Wrap Waveguide_length.

  py::class_<Waveguide_length>(m, "Waveguide_length", "Internal.");

  // Wrap MultiWaveguide.

  py::class_<MultiWaveguide, Waveguide>(m, "MultiWaveguide",
    "A waveguide with several coupled modes (Slab, Circ, Section, ...).")
    .def("field_from_source", &MultiWaveguide::field_from_source,
         py::arg("pos"), py::arg("orientation"),
         "Return the FieldExpansion excited by a dipole current source at "
         "pos with the given orientation.")
    ;

  // Wrap MonoWaveguide.

  py::class_<MonoWaveguide, Waveguide>(m, "MonoWaveguide",
    "A waveguide whose modes do not couple (Planar).");

  // Wrap Scatterer.

  py::class_<Scatterer>(m, "Scatterer",
    "Maps the modes of an incidence waveguide to those of an exit "
    "waveguide with reflection and transmission matrices.")
    .def("calc", &Scatterer::calcRT,
         "Calculate the reflection and transmission matrices.")
    .def("free", &Scatterer::freeRT, "Free the matrices.")
    .def("inc",  &Scatterer::get_inc,
         py::return_value_policy::reference, "Incidence waveguide.")
    .def("ext",  &Scatterer::get_ext,
         py::return_value_policy::reference, "Exit waveguide.")
    .def(py::self + Expression(), py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self + Term(),       py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    ;

  // Wrap MultiScatterer.

  py::class_<MultiScatterer, Scatterer>(m, "MultiScatterer",
    "A Scatterer between MultiWaveguides.");

  // Wrap DenseScatterer.

  py::class_<DenseScatterer, MultiScatterer>(m, "DenseScatterer",
    "A MultiScatterer with full matrices.");

  // Wrap DiagScatterer.

  py::class_<DiagScatterer, MultiScatterer>(m, "DiagScatterer",
    "A MultiScatterer with diagonal matrices.");

  // Wrap MonoScatterer.

  py::class_<MonoScatterer, Scatterer>(m, "MonoScatterer",
    "A Scatterer between MonoWaveguides.");

  // Wrap SquashedScatterer.

  py::class_<SquashedScatterer, DenseScatterer>(m, "SquashedScatterer",
    "A DenseScatterer reduced to a single interface.")
    .def(py::init<DenseScatterer&>(), py::keep_alive<1, 2>(),
         py::arg("scatterer"));

  // Wrap FlippedScatterer.

  py::class_<FlippedScatterer, MultiScatterer>(m, "FlippedScatterer",
    "A MultiScatterer seen from the other side (inc and ext swapped).")
    .def(py::init<MultiScatterer&>(), py::keep_alive<1, 2>(),
         py::arg("scatterer"));

  // Wrap E_Wall.

  py::class_<E_Wall, DiagScatterer>(m, "E_Wall",
    "An electric wall terminating a waveguide in z (the default z boundary "
    "is open). Not a transverse boundary.")
    .def(py::init<Waveguide&>(), py::keep_alive<1, 2>(), py::arg("waveguide"));

  // Wrap H_Wall.

  py::class_<H_Wall, DiagScatterer>(m, "H_Wall",
    "A magnetic wall terminating a waveguide in z (the default z boundary "
    "is open). Not a transverse boundary.")
    .def(py::init<Waveguide&>(), py::keep_alive<1, 2>(), py::arg("waveguide"));

  // Wrap Expression.

  py::class_<Expression>(m, "Expression", R"doc(A sequence of Terms.

Expressions are usually written with ``+`` and ``*``, e.g.
``air(1) + 3*(GaAs(0.1) + AlAs(0.1))``. Build one term by term with
``e = Expression()`` and ``e.add(term)``.)doc")
    .def(py::init<>())
    .def(py::init<const Term&>(),       py::keep_alive<1, 2>(), py::arg("term"))
    .def(py::init<const Expression&>(), py::keep_alive<1, 2>(),
         py::arg("expression"))
    .def("flatten",  &Expression::flatten,
         "Return the expression with nested expressions expanded.")
    .def("inc",      &Expression::get_inc,
         py::return_value_policy::reference, "First waveguide.")
    .def("ext",      &Expression::get_ext,
         py::return_value_policy::reference, "Last waveguide.")
    .def("__repr__", &Expression::repr)
    .def("add",      &Expression::operator+=, py::keep_alive<1, 2>(),
         py::arg("expression"), "Append a Term or Expression.")
    .def("__iadd__", [](py::object self, const Expression& e)
         {self.cast<Expression&>() += e; return self;}, py::keep_alive<1, 2>())
    .def(py::self + py::self, py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self + Term(),   py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self * int(),    py::keep_alive<0, 1>())
    .def(int() * py::self,    py::keep_alive<0, 1>())
    ;

  // Wrap Term.

  py::class_<Term>(m, "Term", R"doc(A building block of expressions.

Examples: ``material(d)``, ``waveguide(d)``, a Stack or Scatterer, ``2*expr``.)doc")
    .def(py::init<Scatterer&>(),        py::keep_alive<1, 2>(),
         py::arg("scatterer"))
    .def(py::init<Stack&>(),            py::keep_alive<1, 2>(),
         py::arg("stack"))
    .def(py::init<const Expression&>(), py::keep_alive<1, 2>(),
         py::arg("expression"))
    .def("inc",      &Term::get_inc,
         py::return_value_policy::reference, "Incidence waveguide.")
    .def("ext",      &Term::get_ext,
         py::return_value_policy::reference, "Exit waveguide.")
    .def("__repr__", &Term::repr)
    .def(py::self + py::self,     py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self + Expression(), py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self * int(),        py::keep_alive<0, 1>())
    .def(int() * py::self,        py::keep_alive<0, 1>())
    ;

  // Wrap Stack.

  py::class_<Stack>(m, "Stack", R"doc(A stack of waveguides along z.

``Stack(wg1(d1) + wg2(d2) + ...)``. The first and last waveguides are the
semi-infinite incidence and exit media (their lengths only matter for field
plots). ``Stack(expression, periods)`` repeats the expression. The
reflection and transmission matrices are computed on demand (``calc``).
Matrix 12 is for light incident from the left (z = 0), 21 from the right
(z = length()).)doc")
    .def(py::init<const Expression&>(),      py::keep_alive<1, 2>(),
         py::arg("expression"))
    .def(py::init<const Expression&, int>(), py::keep_alive<1, 2>(),
         py::arg("expression"), py::arg("periods"))
    .def(py::init<const Term&>(),            py::keep_alive<1, 2>(),
         py::arg("term"))
    .def(py::init([](const Term& t, int n) {return new Stack(Expression(t), n);}),
         py::keep_alive<1, 2>(), py::arg("term"), py::arg("periods"))
    .def("calc",                     &Stack::calcRT,
         "Calculate the reflection and transmission matrices.")
    .def("free",                     &Stack::freeRT,
         "Free the matrices.")
    .def("inc",                      &Stack::get_inc,
         py::return_value_policy::reference, "Incidence waveguide.")
    .def("ext",                      &Stack::get_ext,
         py::return_value_policy::reference, "Exit waveguide.")
    .def("scatterer",                &Stack::as_multi,
         py::return_value_policy::reference_internal,
         "Return the stack as a MultiScatterer.")
    .def("length",                   stack_length, "Length along z.")
    .def("width",                    stack_width,
         "Transverse (c1) size of the incidence waveguide.")
    .def("set_inc_field",            stack_set_inc_field, py::arg("fw"),
         "Set the incident field from the left: a vector of N() mode "
         "amplitudes.")
    .def("set_inc_field",            stack_set_inc_field_2,
         py::arg("fw"), py::arg("bw"),
         "Set the incident fields from the left (fw) and right (bw).")
    .def("set_inc_field_function",   stack_set_inc_field_function,
         py::arg("f"), py::arg("eps"),
         "Set the incident field from a function f(x) (E2 for TE, H2 for TM); "
         "eps is the precision of the overlap integrals. Slabs only.")
    .def("set_inc_field_gaussian",   stack_set_inc_field_gaussian,
         py::arg("amplitude"), py::arg("sigma"), py::arg("x0"), py::arg("eps"),
         "Set a Gaussian incident field amplitude*exp(-((x-x0)/sigma)**2/2); "
         "eps is the precision of the overlap integrals. Slabs only.")
    .def("set_inc_field_plane_wave", stack_set_inc_field_plane_wave,
         py::arg("amplitude"), py::arg("theta"), py::arg("eps"),
         "Set a plane wave incident at angle theta (radians); eps is the "
         "precision of the overlap integrals. Slabs only.")
    .def("inc_field",                &Stack::get_inc_field,
         "Incident field from the left (mode amplitudes).")
    .def("refl_field",               &Stack::get_refl_field,
         "Reflected field at the left (mode amplitudes).")
    .def("trans_field",              &Stack::get_trans_field,
         "Transmitted field at the right (mode amplitudes).")
    .def("inc_S_flux",               stack_inc_S_flux,
         py::arg("c1_start"), py::arg("c1_stop"), py::arg("eps"),
         "Power flux along z at z = 0 between c1_start and c1_stop, relative "
         "precision eps.")
    .def("ext_S_flux",               stack_ext_S_flux,
         py::arg("c1_start"), py::arg("c1_stop"), py::arg("eps"),
         "Power flux along z at z = length() between c1_start and c1_stop, "
         "relative precision eps.")
    .def("field",                    &Stack::field, py::arg("coord"),
         "Field at a Coord, for the incident field set.")
    .def("fw_bw",                    stack_fw_bw, py::arg("z"),
         "Return the forward and backward mode amplitudes at z, as a tuple.")
    .def("fw_bw",                    stack_fw_bw_2,
         py::arg("z"), py::arg("limit"),
         "As fw_bw(z), on the given side (Plus or Min) of an interface at z.")
    .def("lateral_S_flux",           stack_lateral_S_flux, py::arg("c1"),
         "Power flux across x = c1, integrated from z = 0 to length().")
    .def("lateral_S_flux",           stack_lateral_S_flux_2,
         py::arg("c1"), py::arg("k"),
         "Contribution of chunk k to lateral_S_flux(c1).")
    .def("eps",                      &Stack::eps_at, py::arg("coord"),
         "Permittivity at a Coord.")
    .def("mu",                       &Stack::mu_at, py::arg("coord"),
         "Permeability at a Coord.")
    .def("n",                        &Stack::n_at, py::arg("coord"),
         "Refractive index at a Coord.")
    .def("R12",                      &Stack::get_R12,
         "Reflection matrix for incidence from the left.")
    .def("R21",                      &Stack::get_R21,
         "Reflection matrix for incidence from the right.")
    .def("T12",                      &Stack::get_T12,
         "Transmission matrix for incidence from the left.")
    .def("T21",                      &Stack::get_T21,
         "Transmission matrix for incidence from the right.")
    .def("R12",                      stack_R12, py::arg("i"), py::arg("j"),
         "Element (i, j) of R12: reflection from mode j to mode i.")
    .def("R21",                      stack_R21, py::arg("i"), py::arg("j"),
         "Element (i, j) of R21.")
    .def("T12",                      stack_T12, py::arg("i"), py::arg("j"),
         "Element (i, j) of T12: transmission from mode j to mode i.")
    .def("T21",                      stack_T21, py::arg("i"), py::arg("j"),
         "Element (i, j) of T21.")
    .def("R12_power",                &Stack::get_R12_power,
         "Reflected power, mode to mode, for incidence from the left. "
         "BlochSection stacks only.")
    .def("T12_power",                &Stack::get_T12_power,
         "Transmitted power, mode to mode, for incidence from the left. "
         "BlochSection stacks only.")
    .def(py::self + Expression(), py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self + Term(),       py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    ;

  // Scatterers and stacks are accepted wherever a Term is expected.

  py::implicitly_convertible<Scatterer, Term>();
  py::implicitly_convertible<Stack,     Term>();

  // The rest of the wrappers.

  camfr_wrap_2(m);
}

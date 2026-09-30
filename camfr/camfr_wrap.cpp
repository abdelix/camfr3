
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

  py::handle mh = m;

  // Wrap Limit enum.

  py::enum_<Limit> limit(m, "Limit", py::arithmetic());
  limit
    .value("Plus", Plus)
    .value("Min",  Min)
    .export_values();
  name_as_str(limit);

  // Wrap Solver enum.

  py::enum_<Solver> solver(m, "Solver", py::arithmetic());
  solver
    .value("ADR",           ADR)
    .value("track",         track)
    .value("series",        series)
    .value("ASR",           ASR)
    .value("stretched_ASR", stretched_ASR)
    .export_values();
  name_as_str(solver);

  // Wrap Stability enum.

  py::enum_<Stability> stability(m, "Stability", py::arithmetic());
  stability
    .value("normal", normal)
    .value("extra",  extra)
    .value("SVD",    SVD)
    .export_values();
  name_as_str(stability);

  // Wrap Field_calc_heuristic enum.

  py::enum_<Field_calc_heuristic> heuristic
    (m, "Field_calc_heuristic", py::arithmetic());
  heuristic
    .value("identical", identical)
    .value("symmetric", symmetric)
    .export_values();
  name_as_str(heuristic);

  // Wrap Bloch_calc enum.

  py::enum_<Bloch_calc> bloch_calc(m, "Bloch_calc", py::arithmetic());
  bloch_calc
    .value("GEV", GEV)
    .value("T",   T)
    .export_values();
  name_as_str(bloch_calc);

  // Wrap Eigen_calc enum.

  py::enum_<Eigen_calc> eigen_calc(m, "Eigen_calc", py::arithmetic());
  eigen_calc
    .value("lapack",  lapack)
    .value("arnoldi", arnoldi)
    .export_values();
  name_as_str(eigen_calc);

  // Wrap Polarisation enum.

  py::enum_<Polarisation> polarisation(m, "Polarisation", py::arithmetic());
  polarisation
    .value("unknown", unknown)
    .value("TEM",     TEM)
    .value("TE",      TE)
    .value("TM",      TM)
    .value("HE",      HE)
    .value("EH",      EH)
    .value("TE_TM",   TE_TM)
    .export_values();
  name_as_str(polarisation);

  // Wrap Fieldtype enum.

  py::enum_<Fieldtype> fieldtype(m, "Fieldtype", py::arithmetic());
  fieldtype
    .value("cos_type", cos_type)
    .value("sin_type", sin_type)
    .export_values();
  name_as_str(fieldtype);

  // Wrap Section_wall_type enum.

  py::enum_<Section_wall_type> wall_type
    (m, "Section_wall_type", py::arithmetic());
  wall_type
    .value("E_wall",  E_wall)
    .value("H_wall",  H_wall)
    .value("no_wall", no_wall)
    .export_values();
  name_as_str(wall_type);

  // Wrap Sort_type.

  py::enum_<Sort_type> sort_type(m, "Sort_type", py::arithmetic());
  sort_type
    .value("highest_index", highest_index)
    .value("lowest_loss",   lowest_loss)
    .export_values();
  name_as_str(sort_type);

  // Wrap Section_solver enum.

  py::enum_<Section_solver> section_solver
    (m, "Section_solver", py::arithmetic());
  section_solver
    .value("OS",               OS)
    .value("NT",               NT)
    .value("L",                L)
    .value("L_anis",           L_anis)
    .value("ASR_2D",           ASR_2D)
    .value("ASR_2D_stretched", ASR_2D_stretched)
    .export_values();
  name_as_str(section_solver);

  // Wrap Mode_correction enum.

  py::enum_<Mode_correction> mode_correction
    (m, "Mode_correction", py::arithmetic());
  mode_correction
    .value("none",        none)
    .value("snap",        snap)
    .value("guided_only", guided_only)
    .value("full",        full)
    .export_values();
  name_as_str(mode_correction);

  // Wrap getters and setters for global parameters.

  m.def("set_lambda",                 set_lambda);
  m.def("get_lambda",                 get_lambda);
  m.def("set_N",                      set_N);
  m.def("N",                          get_N);
  m.def("set_polarisation",           set_polarisation);
  m.def("get_polarisation",           get_polarisation);
  m.def("set_gain_material",          [mh](py::object mat)
        {mh.attr("_gain_material") = mat;
         set_gain_material(mat.cast<Material*>());});
  m.def("set_solver",                 set_solver);
  m.def("set_stability",              set_stability);
  m.def("set_precision",              set_precision);
  m.def("set_precision_enhancement",  set_precision_enhancement);
  m.def("set_dx_enhanced",            set_dx_enhanced);
  m.def("set_precision_rad",          set_precision_rad);
  m.def("set_C_upperright",           set_C_upperright);
  m.def("set_sweep_from_previous",    set_sweep_from_previous);
  m.def("set_sweep_steps",            set_sweep_steps);
  m.def("set_eps_trace_coarse",       set_eps_trace_coarse);
  m.def("set_chunk_tracing",          set_chunk_tracing);
  m.def("set_unstable_exp_threshold", set_unstable_exp_threshold);
  m.def("set_field_calc_heuristic",   set_field_calc_heuristic);
  m.def("set_bloch_calc",             set_bloch_calc);
  m.def("set_eigen_calc",             set_eigen_calc);
  m.def("set_orthogonal",             set_orthogonal);
  m.def("set_degenerate",             set_degenerate);
  m.def("set_circ_order",             set_circ_order);
  m.def("set_circ_field_type",        set_circ_fieldtype);
  m.def("set_left_wall",              set_left_wall);
  m.def("set_right_wall",             set_right_wall);
  m.def("set_upper_wall",             [mh](py::object w)
        {mh.attr("_upper_wall") = w; set_upper_wall(w.cast<SlabWall*>());});
  m.def("set_lower_wall",             [mh](py::object w)
        {mh.attr("_lower_wall") = w; set_lower_wall(w.cast<SlabWall*>());});
  m.def("set_left_PML",               set_left_PML);
  m.def("set_right_PML",              set_right_PML);
  m.def("set_upper_PML",              set_upper_PML);
  m.def("set_lower_PML",              set_lower_PML);
  m.def("set_circ_PML",               set_circ_PML);
  m.def("set_eta_ASR",                set_eta_ASR);
  m.def("set_section_reduction",      set_section_reduction);
  m.def("set_n_eff_max",              set_n_eff_max);
  m.def("set_NOV",                    set_NOV);
  m.def("set_estimate_cutoff",        set_estimate_cutoff);
  m.def("set_estimate_cutoff_section",set_estimate_cutoff_section);
  m.def("set_low_index_core",         set_low_index_core);
  m.def("set_beta",                   set_beta);
  m.def("set_section_solver",         set_section_solver);
  m.def("set_section_eta_ASR",        set_section_eta_ASR);
  m.def("A_switch",                   A_switch);
  m.def("B_switch",                   B_switch);
  m.def("C_switch",                   C_switch);
  m.def("D_switch",                   D_switch);
  m.def("print_estimates",            print_estimates);
  m.def("set_u_step",                 set_u_step);
  m.def("set_v_step",                 set_v_step);
  m.def("set_percentage_stretched",   set_percentage_stretched);
  m.def("set_extended_output",        set_extended_output);
  m.def("set_keep_all_estimates",     set_keep_all_estimates);
  m.def("set_mode_correction",        set_mode_correction);
  m.def("set_mode_surplus",           set_mode_surplus);
  m.def("set_backward_modes",         set_backward_modes);
  m.def("set_keep_all_1D_estimates",  set_keep_all_1D_estimates);
  m.def("set_fourier_orders",         set_fourier_orders);
  m.def("set_fourier_orders",         [](int Mx) {set_fourier_orders(Mx);});
  m.def("get_fourier_orders_x",       get_fourier_orders_x);
  m.def("get_fourier_orders_y",       get_fourier_orders_y);
  m.def("set_davy",                   set_davy);
  m.def("set_always_recalculate",     set_always_recalculate);
  m.def("set_calc_field_profiles",    set_calc_field_profiles);
  m.def("set_always_dense",           set_always_dense);
  m.def("set_mueller_precision",      set_mueller_precision);
  m.def("free_tmps",                  free_tmps);
  m.def("free_tmp_interfaces",        free_tmp_interfaces);

  // Internal: polyroot, for testsuite/polyroot.py.

  m.def("_polyroot", [](py::array_t<Complex, py::array::forcecast> c)
  {
    auto r = c.unchecked<1>();
    std::vector<Complex> coef(r.data(0), r.data(0) + r.shape(0));
    std::vector<Complex> roots = polyroot(coef);
    py::array_t<Complex> out(roots.size());
    std::copy(roots.begin(), roots.end(), out.mutable_data());
    return out;
  });

  // Wrap Coord.

  py::class_<Coord>(m, "Coord")
    .def(py::init<const Real&, const Real&, const Real&>())
    .def(py::init<const Real&, const Real&, const Real&, Limit>())
    .def(py::init<const Real&, const Real&, const Real&, Limit, Limit>())
    .def(py::init<const Real&, const Real&, const Real&,
                  Limit, Limit, Limit>())
    .def("__repr__", &Coord::repr)
    ;

  // Wrap Field.

  py::class_<Field>(m, "Field")
    .def("E1",       field_E1)
    .def("E2",       field_E2)
    .def("Ez",       field_Ez)
    .def("H1",       field_H1)
    .def("H2",       field_H2)
    .def("Hz",       field_Hz)
    .def("S1",       &Field::S1)
    .def("S2",       &Field::S2)
    .def("Sz",       &Field::Sz)
    .def("abs_E",    &Field::abs_E)
    .def("abs_H",    &Field::abs_H)
    .def("abs_S",    &Field::abs_S)
    .def("__repr__", &Field::repr)
    ;

  // Wrap FieldExpansion.

  py::class_<FieldExpansion>(m, "FieldExpansion")
    .def("field",    &FieldExpansion::field)
    .def("__repr__", &FieldExpansion::repr)
    ;

  // Wrap BaseMaterial.

  py::class_<BaseMaterial>(m, "BaseMaterial");

  // Wrap Material.

  py::class_<Material, BaseMaterial>(m, "Material")
    .def(py::init<const Complex&>())
    .def(py::init<const Complex&, const Complex&>())
    .def("__call__",     material_to_term, py::keep_alive<0, 1>())
    .def("epsr",         material_epsr)
    .def("mur",          material_mur)
    .def("eps",          &Material::eps)
    .def("mu",           &Material::mu)
    .def("n",            &Material::n)
    .def("etar",         &Material::etar)
    .def("eta",          &Material::eta)
    .def("set_epsr_mur", &Material::set_epsr_mur)
    .def("set_epsr",     &Material::set_epsr)
    .def("set_mur",      &Material::set_mur)
    .def("set_n",        &Material::set_n)
    .def("set_etar",     &Material::set_etar)
    .def("gain",         &Material::gain)
    .def("__repr__",     &Material::repr)
    ;

  // Wrap BiaxialMaterial.

  py::class_<BiaxialMaterial, BaseMaterial>(m, "BiaxialMaterial")
    .def(py::init<const Complex&, const Complex&, const Complex&,
                  const Complex&, const Complex&, const Complex&>())
    .def("__call__", material_to_term, py::keep_alive<0, 1>())
    .def("epsr",     basematerial_epsr)
    .def("mur",      basematerial_mur)
    .def("__repr__", &BiaxialMaterial::repr)
    ;

  // Wrap Material_length.

  py::class_<Material_length>(m, "Material_length");

  // Wrap Mode.

  py::class_<Mode>(m, "Mode")
    .def("field",    &Mode::field)
    .def("n_eff",    &Mode::n_eff)
    .def("kz",       &Mode::get_kz)
    .def("pol",      mode_pol)
    .def("__repr__", &Mode::repr)
    ;

  // Wrap Waveguide.

  py::class_<Waveguide>(m, "Waveguide")
    .def("core",     &Waveguide::get_core,
         py::return_value_policy::reference)
    .def("epsr",     &Waveguide::epsr_at)
    .def("mur",      &Waveguide::mur_at)
    .def("eps",      &Waveguide::eps_at)
    .def("mu",       &Waveguide::mu_at)
    .def("n",        &Waveguide::n_at)
    .def("etar",     &Waveguide::etar_at)
    .def("N",        &Waveguide::N)
    .def("mode",     waveguide_get_mode,
         py::return_value_policy::reference_internal)
    .def("fw_mode",  waveguide_get_fw_mode,
         py::return_value_policy::reference_internal)
    .def("bw_mode",  waveguide_get_bw_mode,
         py::return_value_policy::reference_internal)
    .def("calc",     &Waveguide::find_modes)
    .def("__repr__", &Waveguide::repr)
    .def("__call__", waveguide_to_term, py::keep_alive<0, 1>())
    ;

  // Wrap Waveguide_length.

  py::class_<Waveguide_length>(m, "Waveguide_length");

  // Wrap MultiWaveguide.

  py::class_<MultiWaveguide, Waveguide>(m, "MultiWaveguide")
    .def("field_from_source", &MultiWaveguide::field_from_source)
    ;

  // Wrap MonoWaveguide.

  py::class_<MonoWaveguide, Waveguide>(m, "MonoWaveguide");

  // Wrap Scatterer.

  py::class_<Scatterer>(m, "Scatterer")
    .def("calc", &Scatterer::calcRT)
    .def("free", &Scatterer::freeRT)
    .def("inc",  &Scatterer::get_inc,
         py::return_value_policy::reference)
    .def("ext",  &Scatterer::get_ext,
         py::return_value_policy::reference)
    .def(py::self + Expression(), py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self + Term(),       py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    ;

  // Wrap MultiScatterer.

  py::class_<MultiScatterer, Scatterer>(m, "MultiScatterer");

  // Wrap DenseScatterer.

  py::class_<DenseScatterer, MultiScatterer>(m, "DenseScatterer");

  // Wrap DiagScatterer.

  py::class_<DiagScatterer, MultiScatterer>(m, "DiagScatterer");

  // Wrap MonoScatterer.

  py::class_<MonoScatterer, Scatterer>(m, "MonoScatterer");

  // Wrap SquashedScatterer.

  py::class_<SquashedScatterer, DenseScatterer>(m, "SquashedScatterer")
    .def(py::init<DenseScatterer&>(), py::keep_alive<1, 2>());

  // Wrap FlippedScatterer.

  py::class_<FlippedScatterer, MultiScatterer>(m, "FlippedScatterer")
    .def(py::init<MultiScatterer&>(), py::keep_alive<1, 2>());

  // Wrap E_Wall.

  py::class_<E_Wall, DiagScatterer>(m, "E_Wall")
    .def(py::init<Waveguide&>(), py::keep_alive<1, 2>());

  // Wrap H_Wall.

  py::class_<H_Wall, DiagScatterer>(m, "H_Wall")
    .def(py::init<Waveguide&>(), py::keep_alive<1, 2>());

  // Wrap Expression.

  py::class_<Expression>(m, "Expression")
    .def(py::init<>())
    .def(py::init<const Term&>(),       py::keep_alive<1, 2>())
    .def(py::init<const Expression&>(), py::keep_alive<1, 2>())
    .def("flatten",  &Expression::flatten)
    .def("inc",      &Expression::get_inc,
         py::return_value_policy::reference)
    .def("ext",      &Expression::get_ext,
         py::return_value_policy::reference)
    .def("__repr__", &Expression::repr)
    .def("add",      &Expression::operator+=, py::keep_alive<1, 2>())
    .def("__iadd__", [](py::object self, const Expression& e)
         {self.cast<Expression&>() += e; return self;}, py::keep_alive<1, 2>())
    .def(py::self + py::self, py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self + Term(),   py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self * int(),    py::keep_alive<0, 1>())
    .def(int() * py::self,    py::keep_alive<0, 1>())
    ;

  // Wrap Term.

  py::class_<Term>(m, "Term")
    .def(py::init<Scatterer&>(),        py::keep_alive<1, 2>())
    .def(py::init<Stack&>(),            py::keep_alive<1, 2>())
    .def(py::init<const Expression&>(), py::keep_alive<1, 2>())
    .def("inc",      &Term::get_inc,
         py::return_value_policy::reference)
    .def("ext",      &Term::get_ext,
         py::return_value_policy::reference)
    .def("__repr__", &Term::repr)
    .def(py::self + py::self,     py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self + Expression(), py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self * int(),        py::keep_alive<0, 1>())
    .def(int() * py::self,        py::keep_alive<0, 1>())
    ;

  // Wrap Stack.

  py::class_<Stack>(m, "Stack")
    .def(py::init<const Expression&>(),      py::keep_alive<1, 2>())
    .def(py::init<const Expression&, int>(), py::keep_alive<1, 2>())
    .def(py::init<const Term&>(),            py::keep_alive<1, 2>())
    .def(py::init([](const Term& t, int n) {return new Stack(Expression(t), n);}),
         py::keep_alive<1, 2>())
    .def("calc",                     &Stack::calcRT)
    .def("free",                     &Stack::freeRT)
    .def("inc",                      &Stack::get_inc,
         py::return_value_policy::reference)
    .def("ext",                      &Stack::get_ext,
         py::return_value_policy::reference)
    .def("scatterer",                &Stack::as_multi,
         py::return_value_policy::reference_internal)
    .def("length",                   stack_length)
    .def("width",                    stack_width)
    .def("set_inc_field",            stack_set_inc_field)
    .def("set_inc_field",            stack_set_inc_field_2)
    .def("set_inc_field_function",   stack_set_inc_field_function)
    .def("set_inc_field_gaussian",   stack_set_inc_field_gaussian)
    .def("set_inc_field_plane_wave", stack_set_inc_field_plane_wave)
    .def("inc_field",                &Stack::get_inc_field)
    .def("refl_field",               &Stack::get_refl_field)
    .def("trans_field",              &Stack::get_trans_field)
    .def("inc_S_flux",               stack_inc_S_flux)
    .def("ext_S_flux",               stack_ext_S_flux)
    .def("field",                    &Stack::field)
    .def("fw_bw",                    stack_fw_bw)
    .def("fw_bw",                    stack_fw_bw_2)
    .def("lateral_S_flux",           stack_lateral_S_flux)
    .def("lateral_S_flux",           stack_lateral_S_flux_2)
    .def("eps",                      &Stack::eps_at)
    .def("mu",                       &Stack::mu_at)
    .def("n",                        &Stack::n_at)
    .def("R12",                      &Stack::get_R12)
    .def("R21",                      &Stack::get_R21)
    .def("T12",                      &Stack::get_T12)
    .def("T21",                      &Stack::get_T21)
    .def("R12",                      stack_R12)
    .def("R21",                      stack_R21)
    .def("T12",                      stack_T12)
    .def("T21",                      stack_T21)
    .def("R12_power",                &Stack::get_R12_power)
    .def("T12_power",                &Stack::get_T12_power)
    .def(py::self + Expression(), py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    .def(py::self + Term(),       py::keep_alive<0, 1>(), py::keep_alive<0, 2>())
    ;

  // Scatterers and stacks are accepted wherever a Term is expected.

  py::implicitly_convertible<Scatterer, Term>();
  py::implicitly_convertible<Stack,     Term>();

  // The rest of the wrappers.

  camfr_wrap_2(m);
}

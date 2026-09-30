
/////////////////////////////////////////////////////////////////////////////
//
// File:     camfr_wrap_2.cpp
// Author:   Peter.Bienstman@UGent.be
//
// Copyright (C) 2002-2006 Peter Bienstman - Ghent University
//
/////////////////////////////////////////////////////////////////////////////

#include <pybind11/pybind11.h>

#include "camfr_wrap.h"
#include "cavity.h"
#include "bloch.h"
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

inline Complex planar_static_get_kt(Planar& p) {return Planar::get_kt();}
inline void planar_static_set_kt(Planar& p, Complex kt) {Planar::set_kt(kt);}

inline SectionMode* section_get_mode(const Section& s, int i)
{
  check_wg_index(s,i); 
  return dynamic_cast<SectionMode*>(s.get_mode(i+1));
}

inline BlochSectionMode* blochsection_get_mode(const BlochSection& s, int i)
{
  check_wg_index(s,i);
  return dynamic_cast<BlochSectionMode*>(s.get_mode(i+1));
}

inline BlochMode* blochstack_get_mode(BlochStack& b, int i)
{
  check_wg_index(b,i); 
  return dynamic_cast<BlochMode*>(b.get_mode(i+1));
}

inline void cavity_set_current_source(Cavity& c, Coord& pos, Coord& ori) 
  {c.set_source(pos,ori);}

inline void cavity_set_general_source(Cavity& c,
                                      const cVector& fw, const cVector& bw) 
  {c.set_source(fw,bw);}

inline py::tuple blochmode_fw_bw(BlochMode& b, Real z)
{
  cVector fw(global.N,fortranArray);
  cVector bw(global.N,fortranArray);

  b.fw_bw_field(Coord(0,0,z), &fw, &bw);

  return py::make_tuple(fw, bw);
}

inline py::tuple blochmode_fw_bw_2(BlochMode& b, Real z, Limit l)
{
  cVector fw(global.N,fortranArray);
  cVector bw(global.N,fortranArray);

  b.fw_bw_field(Coord(0,0,z,Plus,Plus,l), &fw, &bw);

  return py::make_tuple(fw, bw);
}

inline Real blochstack_length(BlochStack& bs) 
  {return real(bs.get_total_thickness());} 
inline Real blochstack_width(BlochStack& bs)
  {return real(bs.c1_size());}
inline Real cavity_length(Cavity& c) 
  {return real(c.get_bot()->get_total_thickness() 
             + c.get_top()->get_total_thickness());} 
inline Real cavity_width(Cavity& c)
  {return real(c.get_top()->get_inc()->c1_size());}
inline Real slab_width(Slab& s)
  {return real(s.get_width());}
inline Real section_width(Section& s)
  {return real(s.get_width());}
inline Real section_height(Section& s)
  {return real(s.get_height());}
inline Real blochsection_width(BlochSection& s)
  {return real(s.get_width());}
inline Real blochsection_height(BlochSection& s)
  {return real(s.get_height());}
inline Complex blochmode_n(BlochMode& m, Coord &c)
  {return m.get_geom()->n_at(c);}
inline Complex sectionmode_n(SectionMode& m, Coord &c)
  {return m.get_geom()->n_at(c);}


  
/////////////////////////////////////////////////////////////////////////////
//
// Functions dealing with handling of default arguments.
//
/////////////////////////////////////////////////////////////////////////////

Real cavity_calc_sigma(Cavity& c)
  {return c.calc_sigma();}



/////////////////////////////////////////////////////////////////////////////
//
// The following functions are used when expanding an abritrarily shaped 
// field in slabmodes.
//
/////////////////////////////////////////////////////////////////////////////

inline cVector slab_expand_field(Slab& s, py::object o, Real eps)
  {PythonFunction f(o); return s.expand_field(&f, eps);}

inline cVector slab_expand_gaussian
  (Slab& s, Complex height, Complex width, Complex pos, Real eps)
    {GaussianFunction f(height,width,pos); return s.expand_field(&f, eps);}

inline cVector slab_expand_plane_wave
  (Slab& s, const Complex& amplitude, const Complex& angle, Real eps)
{
  Complex index = s.get_core()->n();
  PlaneWaveFunction f(amplitude,angle, index);
  return s.expand_field(&f, eps);
}



/////////////////////////////////////////////////////////////////////////////
//
// More exported functions.
//
/////////////////////////////////////////////////////////////////////////////

void camfr_wrap_2(py::module_& m)
{
  // Wrap Cavity.

  py::class_<Cavity>(m, "Cavity", R"doc(A cavity, cut in two by a plane.

``Cavity(bottom, top)``: the two Stacks are seen from the cavity cut outwards.
Lasing modes are found by varying the wavelength and the gain of the material
set with ``set_gain_material``.)doc")
    .def(py::init<Stack&, Stack&>(), py::keep_alive<1, 2>(), py::keep_alive<1, 3>(),
         py::arg("bottom"), py::arg("top"))
    .def("find_mode",      &Cavity::find_mode,
         py::arg("lambda_start"), py::arg("lambda_stop"),
         py::arg("n_imag_start")=0.0, py::arg("n_imag_stop")=0.015,
         py::arg("passes")=1,
         R"doc(Find a lasing mode in a wavelength and gain interval.

The wavelength and the imaginary index of the gain material are optimised
``passes`` times. The mode is printed, and the cavity field is set to it.)doc")
    .def("find_all_modes", &Cavity::find_modes_in_region,
         py::arg("lambda_start"), py::arg("lambda_stop"),
         py::arg("delta_lambda"),
         py::arg("n_imag_start")=0.0, py::arg("n_imag_stop")=0.015,
         py::arg("passes")=1, py::arg("number")=0,
         "As find_mode, for all lasing modes in the interval (or the "
         "'number' highest ones). Modes closer than delta_lambda are not "
         "resolved.")
    .def("sigma",          cavity_calc_sigma,
         "Smallest singular value of the cavity at the current wavelength and "
         "gain (minimised to find lasing modes).")
    .def("set_source",     cavity_set_current_source,
         py::arg("pos"), py::arg("orientation"),
         "Place a dipole current source in the cavity cut at pos, oriented "
         "along the Coord orientation.")
    .def("set_source",     cavity_set_general_source,
         py::arg("fw"), py::arg("bw"),
         "Place a source in the cavity cut given by forward and backward mode "
         "amplitudes.")
    .def("length",         cavity_length, "Total length along z.")
    .def("width",          cavity_width, "Transverse (c1) size.")
    .def("field",          &Cavity::field, py::arg("coord"),
         "Field at a Coord (after find_mode or set_source).")
    .def("n",              &Cavity::n_at, py::arg("coord"),
         "Refractive index at a Coord.")
    .def("bot_stack",      &Cavity::get_bot,
         py::return_value_policy::reference, "The bottom Stack.")
    .def("top_stack",      &Cavity::get_top,
         py::return_value_policy::reference, "The top Stack.")
    ;

  // Wrap BlochStack.

  py::class_<BlochStack, MultiWaveguide>(m, "BlochStack",
    "An infinite periodic repetition of an expression; its modes are Bloch "
    "modes. N() is 2*N(): forward and backward Bloch waves.")
    .def(py::init<const Expression&>(), py::keep_alive<1, 2>(),
         py::arg("expression"))
    .def("mode",        blochstack_get_mode,
         py::return_value_policy::reference_internal, py::arg("i"),
         "Return BlochMode i.")
    .def("length",      blochstack_length, "Length of one period along z.")
    .def("width",       blochstack_width, "Transverse (c1) size.")
    .def("beta_vector", &BlochStack::get_beta_vector,
         "Propagation constants of all Bloch modes.")
    .def("__repr__",    &BlochStack::repr)
    ;

  // Wrap BlochMode.

  py::class_<BlochMode, Mode>(m, "BlochMode", "A mode of a BlochStack.")
    .def("fw_field", &BlochMode::fw_field,
         "Forward mode amplitudes at the start of the period.")
    .def("bw_field", &BlochMode::bw_field,
         "Backward mode amplitudes at the start of the period.")
    .def("fw_bw",    blochmode_fw_bw, py::arg("z"),
         "Forward and backward mode amplitudes at z, as a tuple.")
    .def("fw_bw",    blochmode_fw_bw_2, py::arg("z"), py::arg("limit"),
         "As fw_bw(z), on the given side (Plus or Min) of an interface at z.")
    .def("S_flux",   &BlochMode::S_flux,
         py::arg("c1_start"), py::arg("c1_stop"), py::arg("eps"),
         "Power flux along z at z = 0 between c1_start and c1_stop, relative "
         "precision eps.")
    .def("n",        blochmode_n, py::arg("coord"),
         "Refractive index at a Coord.")
    ;

  // Wrap InfStack.

  py::class_<InfStack, DenseScatterer>(m, "InfStack",
    "A semi-infinite periodic repetition of an expression, used to "
    "terminate a Stack.")
    .def(py::init<const Expression&>(), py::keep_alive<1, 2>(),
         py::arg("expression"))
    .def("R12", &InfStack::get_R12, "Reflection matrix.")
    ;

  // Wrap RealFunction.

  py::class_<RealFunction>(m, "RealFunction", "A real function (internal).")
    .def("times_called", &RealFunction::times_called,
         "Number of evaluations so far.")
    .def("__call__",     &RealFunction::operator(), py::arg("x"))
    ;

  // Wrap ComplexFunction. (Boost.Python registered it under the name
  // 'RealFunction' too; pybind11 does not allow duplicate names.)

  py::class_<ComplexFunction>(m, "ComplexFunction",
    "A complex function, e.g. a dispersion relation.")
    .def("times_called", &ComplexFunction::times_called,
         "Number of evaluations so far.")
    .def("__call__",     &ComplexFunction::operator(), py::arg("z"))
    ;

  // Wrap Planar.

  py::class_<Planar, MonoWaveguide>(m, "Planar", R"doc(An infinite uniform layer.

Its modes (plane waves at different angles) do not couple, so one propagation
angle is treated at a time: set it with ``set_theta`` or ``set_kt``.)doc")
    .def(py::init<Material&>(), py::keep_alive<1, 2>(), py::arg("material"))
    .def("set_theta", &Planar::set_theta, py::arg("theta"),
         "Set the propagation angle in this layer (radians); Snell's law "
         "fixes it in all other Planars. Set the wavelength first.")
    .def("set_kt",    planar_static_set_kt, py::arg("kt"),
         "Set the transverse wavenumber, common to all Planars.")
    .def("get_kt",    planar_static_get_kt,
         "Return the transverse wavenumber, common to all Planars.")
    ;

  // Wrap Circ.

  py::class_<Circ, MultiWaveguide>(m, "Circ",
    "A cylindrical waveguide inside a perfectly conducting wall, e.g. "
    "Circ(core(r) + clad(R - r)). At most one radial index step is "
    "supported.")
    .def(py::init<Term&>(),       py::keep_alive<1, 2>(), py::arg("term"))
    .def(py::init<Expression&>(), py::keep_alive<1, 2>(), py::arg("expression"))
    ;

  // Wrap SlabWall.

  py::class_<SlabWall>(m, "SlabWall",
    "A transverse boundary condition of a Slab. Predefined: slab_E_wall, "
    "slab_H_wall, slab_no_wall.")
    .def("R", &SlabWall::get_R12, "Reflection coefficient of the wall.")
    ;

  // Wrap SlabWallMixed.

  py::class_<SlabWallMixed, SlabWall>(m, "SlabWallMixed",
    "A wall with the condition a*in + b*out = 0 on the field, i.e. "
    "reflection -a/b.")
    .def(py::init<const Complex&, const Complex&>(), py::arg("a"), py::arg("b"));

  m.attr("slab_E_wall")  = SlabWallMixed(1.0,  1.0);
  m.attr("slab_H_wall")  = SlabWallMixed(1.0, -1.0);
  m.attr("slab_no_wall") = SlabWallMixed(0.0,  1.0);

  // Wrap SlabWall_TBC.

  py::class_<SlabWall_TBC, SlabWall>(m, "SlabWall_TBC",
    "A transparent boundary condition for a given kx and outer material.")
    .def(py::init<const Complex&, const Material&>(), py::keep_alive<1, 3>(),
         py::arg("kx0"), py::arg("material"));

  // Wrap SlabWall_PC.

  py::class_<SlabWall_PC, SlabWall>(m, "SlabWall_PC",
    "A wall formed by a semi-infinite periodic structure (a photonic "
    "crystal) given by one period.")
    .def(py::init<const Expression&>(), py::keep_alive<1, 2>(),
         py::arg("expression"));

  // Wrap SlabDisp.

  py::class_<SlabDisp, ComplexFunction>(m, "SlabDisp",
    "The dispersion relation of a slab, as a function of the transverse "
    "wavenumber kt.")
    .def(py::init<Expression&, Real>(), py::keep_alive<1, 2>(),
         py::arg("expression"), py::arg("wavelength"))
    .def(py::init<Expression&, Real, SlabWall*, SlabWall*>(),
         py::keep_alive<1, 2>(), py::keep_alive<1, 4>(), py::keep_alive<1, 5>(),
         py::arg("expression"), py::arg("wavelength"),
         py::arg("lower_wall"), py::arg("upper_wall"))
    ;

  // Wrap Slab.

  py::class_<Slab, MultiWaveguide>(m, "Slab", R"doc(A 1D layered waveguide.

``Slab(clad(1) + core(0.5) + clad(1))``: layers along x, from x = 0. The
lateral walls are electric by default; change them globally with
``set_lower_wall``/``set_upper_wall``, or per slab with the methods below.)doc")
    .def(py::init<const Term&>(),       py::keep_alive<1, 2>(), py::arg("term"))
    .def(py::init<const Expression&>(), py::keep_alive<1, 2>(),
         py::arg("expression"))
    .def("set_lower_wall",    &Slab::set_lower_wall, py::keep_alive<1, 2>(),
         py::arg("wall"), "Set the wall at x = 0 of this slab.")
    .def("set_upper_wall",    &Slab::set_upper_wall, py::keep_alive<1, 2>(),
         py::arg("wall"), "Set the wall at x = width() of this slab.")
    .def("width",             slab_width, "Width along x.")
    //.def("disp",              &Slab::get_disp)
    .def("expand_field",      slab_expand_field, py::arg("f"), py::arg("eps"),
         "Expand a field profile f(x) in the slab modes; eps is the "
         "precision of the overlap integrals. Returns the mode amplitudes.")
    .def("expand_gaussian",   slab_expand_gaussian,
         py::arg("amplitude"), py::arg("sigma"), py::arg("x0"), py::arg("eps"),
         "Expand amplitude*exp(-((x-x0)/sigma)**2/2) in the slab modes.")
    .def("expand_plane_wave", slab_expand_plane_wave,
         py::arg("amplitude"), py::arg("theta"), py::arg("eps"),
         "Expand a plane wave at angle theta (radians) in the slab modes.")
    .def("set_dummy",         &Slab::set_dummy, py::arg("b"),
         "Internal: mark the slab as a dummy.")
    .def("add_kz2_estimate",  &Slab::add_kz2_estimate, py::arg("kz2"),
         "Add an estimate of kz**2 for the mode search.")
    ;

  // Wrap SectionDisp.

  py::class_<SectionDisp, ComplexFunction>(m, "SectionDisp",
    "The dispersion relation of a Section, as a function of kz.")
    .def(py::init<Stack&, Stack&, Real, int>(),
         py::keep_alive<1, 2>(), py::keep_alive<1, 3>(),
         py::arg("left"), py::arg("right"), py::arg("wavelength"),
         py::arg("M"));

  // Wrap Section. The default M1 and M2 depend on the global settings at
  // the time of the call, so the optional arguments are overloads.

  py::class_<Section, MultiWaveguide>(m, "Section", R"doc(A 2D cross-section (a 3D waveguide).

Built from Slabs along x, e.g. ``Section(side(w1) + center(w2) + side(w1))``,
where each slab is layered along y. ``M1`` is the number of plane waves used
to estimate the modes (default ``N()*mode_surplus``), ``M2`` the number of
slab modes in the dispersion relation (default ``N()``). The estimation takes
most of the solve time; ``set_estimate`` skips it. Two expressions give the
left and right halves of a symmetric section.)doc")
    .def(py::init<Expression&>(),           py::keep_alive<1, 2>(),
         py::arg("expression"))
    .def(py::init<Expression&, int>(),      py::keep_alive<1, 2>(),
         py::arg("expression"), py::arg("M1"))
    .def(py::init<Expression&, int, int>(), py::keep_alive<1, 2>(),
         py::arg("expression"), py::arg("M1"), py::arg("M2"))
    .def(py::init<Expression&, Expression&>(),
         py::keep_alive<1, 2>(), py::keep_alive<1, 3>(),
         py::arg("left"), py::arg("right"))
    .def(py::init<Expression&, Expression&, int>(),
         py::keep_alive<1, 2>(), py::keep_alive<1, 3>(),
         py::arg("left"), py::arg("right"), py::arg("M1"))
    .def(py::init<Expression&, Expression&, int, int>(),
         py::keep_alive<1, 2>(), py::keep_alive<1, 3>(),
         py::arg("left"), py::arg("right"), py::arg("M1"), py::arg("M2"))
    .def(py::init<const Term&>(),           py::keep_alive<1, 2>(),
         py::arg("term"))
    .def("mode",         section_get_mode,
         py::return_value_policy::reference_internal, py::arg("i"),
         "Return SectionMode i.")
    .def("disp",         &Section::get_disp, py::arg("kz"),
         "Evaluate the dispersion relation at kz (zero for a mode).")
    .def("width",        section_width, "Width along x.")
    .def("height",       section_height, "Height along y.")
    .def("eps",          &Section::eps_at, py::arg("coord"),
         "Permittivity at a Coord.")
    .def("mu",           &Section::mu_at, py::arg("coord"),
         "Permeability at a Coord.")
    .def("n",            &Section::n_at, py::arg("coord"),
         "Refractive index at a Coord.")
    .def("set_sorting",  &Section::set_sorting, py::arg("sort"),
         "Order the modes by highest_index or lowest_loss.")
    .def("set_estimate", &Section::set_estimate, py::arg("n_eff"),
         R"doc(Add an estimate of a mode's effective index.

With estimates, the plane-wave estimation stage is skipped and only these
modes are refined: much faster (often 40x). Give one estimate per wanted mode.)doc")
    ;

  // Wrap RefSection.

  py::class_<RefSection, MultiWaveguide>(m, "RefSection",
    "A uniform rectangular section of one material, with analytic modes.")
    .def(py::init<Material&, const Complex&, const Complex&, int>(),
         py::keep_alive<1, 2>(),
         py::arg("material"), py::arg("width"), py::arg("height"),
         py::arg("M"));

  // Wrap SectionMode.

  py::class_<SectionMode, Mode>(m, "SectionMode", "A mode of a Section.")
    .def("n", sectionmode_n, py::arg("coord"), "Refractive index at a Coord.")
    ;

  // Wrap BlochSection.

  py::class_<BlochSection, MultiWaveguide>(m, "BlochSection",
    "A 2D periodic cross-section, solved with plane waves (Fourier orders "
    "from set_fourier_orders).")
    .def(py::init<Expression&>(),  py::keep_alive<1, 2>(),
         py::arg("expression"))
    .def(py::init<const Term&>(),  py::keep_alive<1, 2>(), py::arg("term"))
    .def("mode",          blochsection_get_mode,
         py::return_value_policy::reference_internal, py::arg("i"),
         "Return BlochSectionMode i.")
    .def("width",         blochsection_width, "Width of the period along x.")
    .def("height",        blochsection_height,
         "Height of the period along y.")
    .def("eps",           &BlochSection::eps_at, py::arg("coord"),
         "Permittivity at a Coord.")
    .def("mu",            &BlochSection::mu_at, py::arg("coord"),
         "Permeability at a Coord.")
    .def("n",             &BlochSection::n_at, py::arg("coord"),
         "Refractive index at a Coord.")
    .def("order",         &BlochSection::order,
         py::arg("pol"), py::arg("Mx"), py::arg("My"),
         "Index of the mode with polarisation pol and diffraction order "
         "(Mx, My).")
    .def("set_theta_phi", &BlochSection::set_theta_phi,
         py::arg("theta"), py::arg("phi"),
         "Set the incidence angles (radians).")
    .def("set_kx0_ky0",   &BlochSection::set_kx0_ky0,
         py::arg("kx0"), py::arg("ky0"),
         "Set the transverse Bloch wavenumbers directly.")
    .def("get_kx0",       &BlochSection::get_kx0, "Bloch wavenumber kx0.")
    .def("get_ky0",       &BlochSection::get_ky0, "Bloch wavenumber ky0.")
    ;

  // Wrap BlochSectionMode.

  py::class_<BlochSectionMode, Mode>(m, "BlochSectionMode",
    "A mode (diffraction order) of a BlochSection.")
    .def("get_Mx",   &BlochSectionMode::get_Mx, "Diffraction order in x.")
    .def("get_My",   &BlochSectionMode::get_My, "Diffraction order in y.")
    .def("get_kx",   &BlochSectionMode::get_kx, "Wavenumber kx.")
    .def("get_ky",   &BlochSectionMode::get_ky, "Wavenumber ky.")
    ;
}

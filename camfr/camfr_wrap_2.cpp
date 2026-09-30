
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

  py::class_<Cavity>(m, "Cavity")
    .def(py::init<Stack&, Stack&>())
    .def("find_mode",      &Cavity::find_mode,
         py::arg("lambda_start"), py::arg("lambda_stop"),
         py::arg("n_imag_start")=0.0, py::arg("n_imag_stop")=0.015,
         py::arg("passes")=1)
    .def("find_all_modes", &Cavity::find_modes_in_region,
         py::arg("lambda_start"), py::arg("lambda_stop"),
         py::arg("delta_lambda"),
         py::arg("n_imag_start")=0.0, py::arg("n_imag_stop")=0.015,
         py::arg("passes")=1, py::arg("number")=0)
    .def("sigma",          cavity_calc_sigma)
    .def("set_source",     cavity_set_current_source)
    .def("set_source",     cavity_set_general_source)
    .def("length",         cavity_length)
    .def("width",          cavity_width)
    .def("field",          &Cavity::field)
    .def("n",              &Cavity::n_at)
    .def("bot_stack",      &Cavity::get_bot,
         py::return_value_policy::reference)
    .def("top_stack",      &Cavity::get_top,
         py::return_value_policy::reference)
    ;

  // Wrap BlochStack.

  py::class_<BlochStack, MultiWaveguide>(m, "BlochStack")
    .def(py::init<const Expression&>())
    .def("mode",        blochstack_get_mode,
         py::return_value_policy::reference)
    .def("length",      blochstack_length)
    .def("width",       blochstack_width)
    .def("beta_vector", &BlochStack::get_beta_vector)
    .def("__repr__",    &BlochStack::repr)
    ;

  // Wrap BlochMode.

  py::class_<BlochMode, Mode>(m, "BlochMode")
    .def("fw_field", &BlochMode::fw_field)
    .def("bw_field", &BlochMode::bw_field)
    .def("fw_bw",    blochmode_fw_bw)
    .def("fw_bw",    blochmode_fw_bw_2)
    .def("S_flux",   &BlochMode::S_flux)
    .def("n",        blochmode_n)
    ;

  // Wrap InfStack.

  py::class_<InfStack, DenseScatterer>(m, "InfStack")
    .def(py::init<const Expression&>())
    .def("R12", &InfStack::get_R12)
    ;

  // Wrap RealFunction.

  py::class_<RealFunction>(m, "RealFunction")
    .def("times_called", &RealFunction::times_called)
    .def("__call__",     &RealFunction::operator())
    ;

  // Wrap ComplexFunction. (Boost.Python registered it under the name
  // 'RealFunction' too; pybind11 does not allow duplicate names.)

  py::class_<ComplexFunction>(m, "ComplexFunction")
    .def("times_called", &ComplexFunction::times_called)
    .def("__call__",     &ComplexFunction::operator())
    ;

  // Wrap Planar.

  py::class_<Planar, MonoWaveguide>(m, "Planar")
    .def(py::init<Material&>())
    .def("set_theta", &Planar::set_theta)
    .def("set_kt",    planar_static_set_kt)
    .def("get_kt",    planar_static_get_kt)
    ;

  // Wrap Circ.

  py::class_<Circ, MultiWaveguide>(m, "Circ")
    .def(py::init<Term&>())
    .def(py::init<Expression&>())
    ;

  // Wrap SlabWall.

  py::class_<SlabWall>(m, "SlabWall")
    .def("R", &SlabWall::get_R12)
    ;

  // Wrap SlabWallMixed.

  py::class_<SlabWallMixed, SlabWall>(m, "SlabWallMixed")
    .def(py::init<const Complex&, const Complex&>());

  m.attr("slab_E_wall")  = SlabWallMixed(1.0,  1.0);
  m.attr("slab_H_wall")  = SlabWallMixed(1.0, -1.0);
  m.attr("slab_no_wall") = SlabWallMixed(0.0,  1.0);

  // Wrap SlabWall_TBC.

  py::class_<SlabWall_TBC, SlabWall>(m, "SlabWall_TBC")
    .def(py::init<const Complex&, const Material&>());

  // Wrap SlabWall_PC.

  py::class_<SlabWall_PC, SlabWall>(m, "SlabWall_PC")
    .def(py::init<const Expression&>());

  // Wrap SlabDisp.

  py::class_<SlabDisp, ComplexFunction>(m, "SlabDisp")
    .def(py::init<Expression&, Real>())
    .def(py::init<Expression&, Real, SlabWall*, SlabWall*>())
    ;

  // Wrap Slab.

  py::class_<Slab, MultiWaveguide>(m, "Slab")
    .def(py::init<const Term&>())
    .def(py::init<const Expression&>())
    .def("set_lower_wall",    &Slab::set_lower_wall)
    .def("set_upper_wall",    &Slab::set_upper_wall)
    .def("width",             slab_width)
    //.def("disp",              &Slab::get_disp)
    .def("expand_field",      slab_expand_field)
    .def("expand_gaussian",   slab_expand_gaussian)
    .def("expand_plane_wave", slab_expand_plane_wave)
    .def("set_dummy",         &Slab::set_dummy)
    .def("add_kz2_estimate",  &Slab::add_kz2_estimate)
    ;

  // Wrap SectionDisp.

  py::class_<SectionDisp, ComplexFunction>(m, "SectionDisp")
    .def(py::init<Stack&, Stack&, Real, int>());

  // Wrap Section. The default M1 and M2 depend on the global settings at
  // the time of the call, so the optional arguments are overloads.

  py::class_<Section, MultiWaveguide>(m, "Section")
    .def(py::init<Expression&>())
    .def(py::init<Expression&, int>())
    .def(py::init<Expression&, int, int>())
    .def(py::init<Expression&, Expression&>())
    .def(py::init<Expression&, Expression&, int>())
    .def(py::init<Expression&, Expression&, int, int>())
    .def(py::init<const Term&>())
    .def("mode",         section_get_mode,
         py::return_value_policy::reference)
    .def("disp",         &Section::get_disp)
    .def("width",        section_width)
    .def("height",       section_height)
    .def("eps",          &Section::eps_at)
    .def("mu",           &Section::mu_at)
    .def("n",            &Section::n_at)
    .def("set_sorting",  &Section::set_sorting)
    .def("set_estimate", &Section::set_estimate)
    ;

  // Wrap RefSection.

  py::class_<RefSection, MultiWaveguide>(m, "RefSection")
    .def(py::init<Material&, const Complex&, const Complex&, int>());

  // Wrap SectionMode.

  py::class_<SectionMode, Mode>(m, "SectionMode")
    .def("n", sectionmode_n)
    ;

  // Wrap BlochSection.

  py::class_<BlochSection, MultiWaveguide>(m, "BlochSection")
    .def(py::init<Expression&>())
    .def(py::init<const Term&>())
    .def("mode",          blochsection_get_mode,
         py::return_value_policy::reference)
    .def("width",         blochsection_width)
    .def("height",        blochsection_height)
    .def("eps",           &BlochSection::eps_at)
    .def("mu",            &BlochSection::mu_at)
    .def("n",             &BlochSection::n_at)
    .def("order",         &BlochSection::order)
    .def("set_theta_phi", &BlochSection::set_theta_phi)
    .def("set_kx0_ky0",   &BlochSection::set_kx0_ky0)
    .def("get_kx0",       &BlochSection::get_kx0)
    .def("get_ky0",       &BlochSection::get_ky0)
    ;

  // Wrap BlochSectionMode.

  py::class_<BlochSectionMode, Mode>(m, "BlochSectionMode")
    .def("get_Mx",   &BlochSectionMode::get_Mx)
    .def("get_My",   &BlochSectionMode::get_My)
    .def("get_kx",   &BlochSectionMode::get_kx)
    .def("get_ky",   &BlochSectionMode::get_ky)
    ;
}

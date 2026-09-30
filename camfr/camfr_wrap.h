
/////////////////////////////////////////////////////////////////////////////
//
// File:     camfr_wrap.h
// Author:   Peter.Bienstman@UGent.be
//
// Copyright (C) 1998-2006 Peter Bienstman - Ghent University
//
/////////////////////////////////////////////////////////////////////////////

#ifndef CAMFR_WRAP_H
#define CAMFR_WRAP_H

#include <stdexcept>

#include <pybind11/pybind11.h>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>

#include "defs.h"
#include "waveguide.h"
#include "math/linalg/linalg.h"
#include "math/calculus/function.h"

namespace py = pybind11;

/////////////////////////////////////////////////////////////////////////////
//
// check_index
//
/////////////////////////////////////////////////////////////////////////////

// pybind11 turns std::out_of_range into IndexError.

inline void check_index(int i)
{
  if ( (i<0) || (i>=int(global.N)) )
    throw std::out_of_range("index out of bounds.");
}



/////////////////////////////////////////////////////////////////////////////
//
// check_wg_index
//
/////////////////////////////////////////////////////////////////////////////

inline void check_wg_index(const Waveguide& w, int i)
{
  if ( (i<0) || (i>=int(w.N())) )
    throw std::out_of_range("index out of bounds.");
}



/////////////////////////////////////////////////////////////////////////////
//
// Conversion of cVector and cMatrix to and from NumPy arrays.
//
//   Python -> cVector: a 1D array of length global.N, cast to complex.
//   cVector, cMatrix -> Python: a new complex array (a copy).
//
//   The Blitz arrays use Fortran storage with base 1, hence the i+1.
//   Declared here so that both wrapper files use the same casters.
//
/////////////////////////////////////////////////////////////////////////////

namespace pybind11 { namespace detail {

template <> struct type_caster<cVector>
{
  public:

    PYBIND11_TYPE_CASTER(cVector, const_name("numpy.ndarray[complex128]"));

    bool load(handle src, bool)
    {
      if (!isinstance<array>(src))
        return false;

      array a = reinterpret_borrow<array>(src);

      if ( (a.ndim() != 1) || (a.shape(0) != py::ssize_t(global.N)) )
        return false;

      auto c = array_t<Complex, array::forcecast>::ensure(src);
      if (!c)
        return false;

      auto r = c.unchecked<1>();

      cVector v(global.N, fortranArray);
      for (int i=0; i<int(global.N); i++)
        v(i+1) = r(i);

      value.reference(v);

      return true;
    }

    static handle cast(const cVector& c, return_value_policy, handle)
    {
      array_t<Complex> result(c.rows());
      auto r = result.mutable_unchecked<1>();

      for (int i=0; i<c.rows(); i++)
        r(i) = c(i+1);

      return result.release();
    }
};

template <> struct type_caster<cMatrix>
{
  public:

    PYBIND11_TYPE_CASTER(cMatrix, const_name("numpy.ndarray[complex128]"));

    bool load(handle, bool)
      {return false;} // Not used: no function takes a cMatrix argument.

    static handle cast(const cMatrix& c, return_value_policy, handle)
    {
      array_t<Complex> result({py::ssize_t(c.rows()),
                               py::ssize_t(c.columns())});
      auto r = result.mutable_unchecked<2>();

      for (int i=0; i<c.rows(); i++)
        for (int j=0; j<c.columns(); j++)
          r(i,j) = c(i+1,j+1);

      return result.release();
    }
};

}} // namespace pybind11::detail



/////////////////////////////////////////////////////////////////////////////
//
// The following classes are used when expanding an abritrarily shaped 
// field in slabmodes.
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// PythonFunction
//
/////////////////////////////////////////////////////////////////////////////

class PythonFunction : public ComplexFunction
{
  public:

    PythonFunction(py::object f_): f(f_) {}

    Complex operator()(const Complex& z)
      {counter++; return f(z).cast<Complex>();}

  protected:

    py::object f;
};



/////////////////////////////////////////////////////////////////////////////
//
// GaussianFunction
//
/////////////////////////////////////////////////////////////////////////////

class GaussianFunction : public ComplexFunction
{
  public:

    GaussianFunction (Complex height, Complex width, Complex position)
      : h(height), w(width), p(position) {}

    Complex operator()(const Complex& x)
	{counter++; return h*exp(-(x-p)*(x-p)/(w*w*2.0));}

  protected:

    Complex h, w, p;
};



/////////////////////////////////////////////////////////////////////////////
//
// PlaneWaveFunction
//
/////////////////////////////////////////////////////////////////////////////

class PlaneWaveFunction : public ComplexFunction
{
  public:

    PlaneWaveFunction (Complex amplitude, Complex angle, Complex index)
      : am(amplitude), an(angle), n(index) {}

    Complex operator()(const Complex& x)
      {counter++; return am*exp(-2.0*I*pi*sin(an)*n*x/global.lambda);}

  protected:

    Complex am, an, n;
};



#endif



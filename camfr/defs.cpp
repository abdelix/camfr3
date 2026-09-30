
/////////////////////////////////////////////////////////////////////////////
//
// File:     defs.cpp
// Author:   Peter.Bienstman@rug.ac.be
// Date:     19980824
// Version:  1.0
//
// Copyright (C) 1998 Peter Bienstman - Ghent University
//
/////////////////////////////////////////////////////////////////////////////

#include <Python.h>
#include <limits>
#include <iostream>
#include "defs.h"

Global global={0,0,TE,0,track,normal,100,1,0.01,100,100,Complex(1,1),false,
               20,1e-14,true,1e-12,identical,GEV,lapack,true,true,false,
               0.0,1.2,false,false,false,true,false,1e-14};

/////////////////////////////////////////////////////////////////////////////
//
// operator<< for Polarisation
//
/////////////////////////////////////////////////////////////////////////////

const std::string Pol_string[] 
  = {"unknown", "TEM", "TE", "TM", "HE", "EH", "TE_TM"};

std::ostream& operator<< (std::ostream& s, const Polarisation& pol)
{
  return s << Pol_string[pol];
};



/////////////////////////////////////////////////////////////////////////////
//
// Python print functions.
//
/////////////////////////////////////////////////////////////////////////////

void py_print(const std::string& s) {PySys_WriteStdout("%s\n",s.c_str());}
void py_error(const std::string& s) {PySys_WriteStderr("%s\n",s.c_str());}



/////////////////////////////////////////////////////////////////////////////
//
// machine_eps
//
//   smallest number x for which 1+x != 1
//
//   Used to be computed at run time with a copy of MACHAR (W. J. Cody,
//   ACM TOMS 14, 1988); numeric_limits gives the identical value
//   (PORTING_JOURNAL.md, entry 58).
//
/////////////////////////////////////////////////////////////////////////////

Real machine_eps()
  {return std::numeric_limits<Real>::epsilon();}


/////////////////////////////////////////////////////////////////////////////
//
// out_of_memory error handler
//
/////////////////////////////////////////////////////////////////////////////

void out_of_memory()
{
  py_error("Fatal error: Out of memory.");
  exit(-1);
}



/////////////////////////////////////////////////////////////////////////////
//
// pick_sign_k
//
/////////////////////////////////////////////////////////////////////////////

void pick_sign_k(Complex* k)
{
  // Lossy only.

  if (imag(*k) > 0)
    *k = - *k;

  if (abs(imag(*k)) < 1e-12)
    if (real(*k) < 0)
      *k = - *k;

  return;

  // Old style.

  if (real(*k) < 0)
    *k = - *k;

  if (abs(real(*k)) < 1e-8)
    if (imag(*k) > 0)
      *k = - *k;
}



/////////////////////////////////////////////////////////////////////////////
//
// sqrt_45
//
/////////////////////////////////////////////////////////////////////////////

Complex sqrt_45(const Complex& kz2)
{
  Complex kz = sqrt(kz2);
  
  if (imag(kz) > 0)
    kz = -kz;

  if (abs(imag(kz)) < abs(real(kz)))
    if (real(kz) < 0)
      kz = -kz;

  return kz;
}



/////////////////////////////////////////////////////////////////////////////
//
// File:     patterson_z_n.cpp
// Author:   Peter.Bienstman@rug.ac.be
// Date:     20000320
// Version:  1.0
//
// Copyright (C) 2001 Peter Bienstman - Ghent University
//
// camfr3: the formula evaluation in patterson_z_n was rewritten in 2026 on
// the basis of the JPL MATH77 library (BSD licence), replacing a
// translation of ACM TOMS Algorithm 699 (PORTING_JOURNAL.md, entry 64).
// See ../quadrature/patterson_rule.h.
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "../function.h"
#include "../quadrature/patterson_rule.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// patterson_z_n
//
//   Integrates z^n/f(z), n = 0..M, along the segment from a to b with the
//   Patterson formulas. The segment is parameterised by t in [0,1], so the
//   formulas are applied on [0,1] and the results scaled by b-a. All
//   moments are computed from the same samples of 1/f; the formulas stop
//   when every moment has converged.
//
/////////////////////////////////////////////////////////////////////////////

vector<Complex> patterson_z_n(ComplexFunction& f,
                              const Complex& a, const Complex& b, int M,
                              Real eps, Real mu, bool* error_ptr,
                              unsigned int max_k,
                              vector<Complex>* abs_error)
{
  // Check if a and b are different.

  if (1. + abs(a-b) <= 1.)
  {
    vector<Complex> result;
    for (unsigned int i=0; i<=M; i++)
      result.push_back(0.0);
    return result;
  }

  // Check and coerce k.

  if (max_k < 2)
  {
    py_print("Warning: increasing max_k to 2.");
    max_k = 3;
  }

  if (max_k > patterson_max_k)
  {
    py_print("Warning: restricting max_k to 8.");
    max_k = patterson_max_k;
  }

  const Real* p = patterson_p;
  const Complex delta = b-a;

  // Moments of the samples kept for later formulas (slots 1..17), and the
  // current and previous estimates of the moments on [0,1]. M decreases
  // when round-off makes the higher moments meaningless (see header).

  vector<vector<Complex> > stored(patterson_n_stored+1,
                                  vector<Complex>(M+1));
  vector<Complex> estimate(M+1), previous(M+1), sum(M+1);

  // 1-point formula: the midpoint rule.

  const Complex z_mid = a + 0.5*delta;
  Complex moment = 1.0 / f(z_mid);

  stored[1][0] = estimate[0] = moment;

  for (int n=1; n<=M; n++)
  {
    moment *= z_mid;
    stored[1][n] = estimate[n] = moment;

    if (machine_eps()*abs(moment) > mu)
      M = n-1;
  }

  int n_new = 1; // New node pairs in the current formula.
  int ip = 1;    // Next coefficient in p.

  for (unsigned int k=2; k<=max_k; k++, n_new*=2)
  {
    const PattersonStep& step = patterson_steps[k];

    for (int n=0; n<=M; n++)
    {
      previous[n] = estimate[n];
      sum[n] = 0.0;
    }

    // Corrections to the stored samples.

    for (int r=0; r<step.n_ranges; r++)
      for (int i=step.lo[r]; i<=step.hi[r]; i++, ip++)
        for (int n=0; n<=M; n++)
          sum[n] += p[ip]*stored[i][n];

    // New node pairs at t and 1-t.

    int slot = step.store_first;

    for (int i=0; i<n_new; i++, slot++)
    {
      const Real t = p[ip++]*0.5;
      const Real w = p[ip++];

      const Complex z1 = a +      t *delta;
      const Complex z2 = a + (1.0-t)*delta;

      Complex m1 = 1.0 / f(z1);
      Complex m2 = 1.0 / f(z2);

      for (int n=0; n<=M; n++)
      {
        if (n > 0)
        {
          m1 *= z1;
          m2 *= z2;
        }

        const Complex m_pair = m1 + m2;

        sum[n] += w*m_pair;

        if (slot <= step.store_last)
          stored[slot][n] = m_pair;

        if ( (n > 0) && ( (machine_eps()*abs(m1) > mu)
                       || (machine_eps()*abs(m2) > mu) ) )
          M = n-1;
      }
    }

    // New estimates on [0,1]; converged when all moments have.

    bool converged = true;

    for (int n=0; n<=M; n++)
    {
      estimate[n] = 0.5*(sum[n] + previous[n]);

      if (abs(estimate[n]-previous[n]) > abs(eps*estimate[n]))
        converged = false;
    }

    if (converged)
    {
      if (error_ptr)
        *error_ptr = false;

      goto done;
    }
  }

  // Failed to converge.

  if (error_ptr)
    *error_ptr = true;

  // Scale from [0,1] to the segment.

  done:

  if (abs_error)
  {
    abs_error->clear();
    for (int n=0; n<=M; n++)
      abs_error->push_back(delta*(estimate[n]-previous[n]));
  }

  vector<Complex> result;
  for (int n=0; n<=M; n++)
    result.push_back(delta*estimate[n]);

  return result;
}



/////////////////////////////////////////////////////////////////////////////
//
// patterson_quad_z_n_sub
//
//   Helper routine for patterson_quad_z_n.
//   Integrates subinterval, but uses a relaxed convergence criterion based
//   on the estimated value of the integral over the entire interval.
//
/////////////////////////////////////////////////////////////////////////////

vector<Complex> patterson_quad_z_n_sub
  (ComplexFunction& f, const Complex& a, const Complex& b, int M,
   Real eps, Real mu, const vector<Complex>& result_estimate,
   unsigned int max_k)
{ 
  bool error;
  vector<Complex> abs_error;
  vector<Complex> result
    = patterson_z_n(f,a,b,M,eps,mu,&error,max_k,&abs_error);

  if (error == false)
    return result;

  // Check if relaxed convergence is satisfied.
  //
  // patterson_z_n lowers M when the higher moments would lose precision,
  // and that depends on the interval, so the estimate for the entire
  // interval can have fewer moments than this subinterval. For those, fall
  // back on the subinterval's own result. (It used to read past the end of
  // result_estimate.)

  bool converged = true;

  for (unsigned int i=0; i<abs_error.size(); i++)
  {
    const Complex scale = (i < result_estimate.size()) ? result_estimate[i]
                                                        : result[i];
    if ( abs(abs_error[i]) > abs(scale * eps) )
      converged = false;
  }

  if (converged)
    return result;

  // Do adaptive subdivision of interval.

  vector<Complex> result1
    = patterson_quad_z_n_sub(f, a, (a+b)/2., M,eps,mu,result_estimate,max_k);

  vector<Complex> result2
    = patterson_quad_z_n_sub(f, (a+b)/2., b, M,eps,mu,result_estimate,max_k);

  unsigned int new_M
    = (result1.size()>result2.size()) ? result2.size() : result1.size();

  vector<Complex> new_result;
  for (unsigned int i=0; i<new_M; i++)
    new_result.push_back(result1[i] + result2[i]);

  return new_result;
}



/////////////////////////////////////////////////////////////////////////////
//
// patterson_quad_z_n
//
/////////////////////////////////////////////////////////////////////////////

vector<Complex> patterson_quad_z_n(ComplexFunction& f,
                                   const Complex& a, const Complex& b, int M,
                                   Real eps, Real mu, unsigned int max_k)
{
  // Try patterson on the entire interval.

  bool error;
  vector<Complex> result 
    = patterson_z_n(f, a, b, M, eps, mu, &error, max_k, NULL);

  if (error == false)
    return result;

  // Do adaptive subdivision of interval
  
  vector<Complex> result1
    = patterson_quad_z_n_sub(f, a, (a+b)/2., M, eps, mu, result, max_k);

  vector<Complex> result2
    = patterson_quad_z_n_sub(f, (a+b)/2., b, M, eps, mu, result, max_k);

  unsigned int new_M
    = (result1.size()>result2.size()) ? result2.size() : result1.size();

  vector<Complex> new_result;
  for (unsigned int i=0; i<new_M; i++)
    new_result.push_back(result1[i] + result2[i]);

  return new_result;
}

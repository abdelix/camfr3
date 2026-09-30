
/////////////////////////////////////////////////////////////////////////////
//
// File:     patterson.cpp
// Author:   Peter.Bienstman@rug.ac.be
// Date:     20000320
// Version:  1.0
//
// Copyright (C) 2001 Peter Bienstman - Ghent University
//
// camfr3: the formula evaluation was rewritten in 2026 on the basis of the
// JPL MATH77 library (BSD licence), replacing a translation of ACM TOMS
// Algorithm 699 (PORTING_JOURNAL.md, entry 64). See patterson_rule.h.
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "patterson.h"
#include "patterson_rule.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// patterson
//
//   Applies the Patterson formulas with 1, 3, 7, ... nodes in turn until
//   two successive estimates agree to a relative precision eps.
//
/////////////////////////////////////////////////////////////////////////////

Real patterson(RealFunction& f, Real a, Real b, Real eps,
               bool* error_ptr, unsigned int max_k,
               Real* abs_error)
{
  // Check if a and b are different.

  if (1. + abs(a-b) <= 1.)
    return 0.0;

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
  const Real diff = 0.5*(b-a);

  // Samples kept for the corrections of later formulas (slots 1..17).

  Real stored[patterson_n_stored+1];

  // 1-point formula: the midpoint rule. (Not 0.5*(a+b), in case the
  // arithmetic is not binary.)

  stored[1] = f(a+diff);

  Real estimate = (b-a)*stored[1];
  Real previous = estimate;

  int ip = 1;    // Next coefficient in p.
  int n_new = 1; // New node pairs in the current formula.

  for (unsigned int k=2; k<=max_k; k++, n_new*=2)
  {
    const PattersonStep& step = patterson_steps[k];

    previous = estimate;

    // Corrections to the stored samples.

    Real sum = 0.0;

    for (int r=0; r<step.n_ranges; r++)
      for (int i=step.lo[r]; i<=step.hi[r]; i++)
        sum += p[ip++]*stored[i];

    // New node pairs, symmetric about the midpoint.

    int slot = step.store_first;

    for (int i=0; i<n_new; i++)
    {
      const Real x = p[ip++]*diff;
      const Real f_pair = f(a+x) + f(b-x);

      sum += p[ip++]*f_pair;

      if (slot <= step.store_last)
        stored[slot++] = f_pair;
    }

    estimate = diff*sum + 0.5*previous;

    // Converged?

    if (abs(estimate-previous) <= abs(eps*estimate))
    {
      if (error_ptr)
        *error_ptr = false;

      if (abs_error)
        *abs_error = estimate - previous;

      return estimate;
    }
  }

  // Failed to converge.

  if (error_ptr)
    *error_ptr = true;

  if (abs_error)
    *abs_error = estimate - previous;

  return estimate;
}

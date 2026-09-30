
/////////////////////////////////////////////////////////////////////////////
//
// File:     polyroot.cpp
// Author:   Peter.Bienstman@rug.ac.be
// Date:     20000327
// Version:  1.0
//
// Copyright (C) 2001 Peter Bienstman - Ghent University
//
/////////////////////////////////////////////////////////////////////////////

#include "polyroot.h"
#include "../../linalg/linalg.h"

using std::vector;

/////////////////////////////////////////////////////////////////////////////
//
// polyroot
//
//   Roots of a polynomial, with the coefficients ordered by decreasing
//   powers of z, computed as the eigenvalues of the companion matrix (the
//   method of numpy.roots and MATLAB's roots). LAPACK's zgeev balances the
//   matrix first, which makes this backward stable in the coefficients.
//
//   Replaces the wrapper around Jenkins-Traub (cpoly, ACM Algorithm 419),
//   whose licence restricts commercial use (PORTING_JOURNAL.md, entry 56).
//
/////////////////////////////////////////////////////////////////////////////

vector<Complex> polyroot(const vector<Complex>& coef)
{
  vector<Complex> results;

  // Skip leading zero coefficients: they do not change the roots.

  unsigned int first = 0;
  while ( (first < coef.size()) && (abs(coef[first]) == 0.0) )
    first++;

  if (coef.size() - first < 2) // Constant polynomial: no roots.
    return results;

  const int N = coef.size() - first - 1;

  // Companion matrix of the monic polynomial
  //   z^N + c_1 z^(N-1) + ... + c_N:
  // first row -c_1 ... -c_N, ones on the subdiagonal.

  cMatrix A(N,N,fortranArray);
  A = 0.0;

  for (int j=1; j<=N; j++)
    A(1,j) = -coef[first+j] / coef[first];

  for (int i=2; i<=N; i++)
    A(i,i-1) = 1.0;

  cVector e(N,fortranArray);
  e.reference(eigenvalues(A));

  for (int i=1; i<=N; i++)
    results.push_back(e(i));

  return results;
}

/////////////////////////////////////////////////////////////////////////////
//
// File:     patterson_rule.h
//
// The sequence of Patterson quadrature formulas (T. N. L. Patterson,
// Math. Comp. 22, 847-856, 1968) with 1, 3, 7, 15, 31, 63, 127 and 255
// nodes, in the representation of F. T. Krogh and W. V. Snyder: each
// formula reuses the function values of the previous ones. It is used by
// patterson() (real integrals) and patterson_z_n() (complex moments).
//
// Formula k (k = 2..8) is evaluated on [a,b], with diff = (b-a)/2, as
//
//   I_k = I_{k-1}/2 + diff*(  sum of p[...]*stored samples
//                           + sum of p[...]*(f(a+x)+f(b-x))  ),
//
// reading the coefficients p = patterson_p sequentially from index 1:
//
//   - first one correction for every stored sample in the ranges of
//     patterson_steps[k].lo/hi (I_1 = 2*diff*f(midpoint) is sample 1);
//   - then, for each of the 2^(k-2) new node pairs, the offset x/diff of
//     the node from the ends of the interval (1 - |node| on [-1,1]),
//     followed by its weight. The sums f(a+x)+f(b-x) of the first new
//     pairs are stored in slots store_first..store_last for later
//     formulas.
//
// The coefficients (patterson_coeff.cpp) and the index scheme below come
// from dinta.f of the JPL MATH77 library: the scheme is its tables KORECT
// and FSTORE, decoded. They are used under this licence:
//

//
// camfr3: written 2026 to replace a translation of ACM TOMS Algorithm 699
// (PORTING_JOURNAL.md, entry 64).
//
/////////////////////////////////////////////////////////////////////////////

#ifndef PATTERSON_RULE_H
#define PATTERSON_RULE_H

#include "../../../defs.h"

// Coefficients, indexed 1..305 (patterson_coeff.cpp).

extern const Real patterson_p[306];

// Highest formula (255 nodes) and number of slots for stored samples.

const int patterson_max_k    = 8;
const int patterson_n_stored = 17;

struct PattersonStep
{
  int n_ranges;      // Ranges of stored samples that get a correction.
  int lo[3];
  int hi[3];
  int store_first;   // Slots for the first new samples; none if
  int store_last;    // store_first > store_last.
};

// Indexed by k; entries 0 and 1 are unused.

static const PattersonStep patterson_steps[patterson_max_k+1] =
{
  {0, {0},       {0},        0,  0},
  {0, {0},       {0},        0,  0},
  {1, {1},       {1},        2,  2}, //   3 nodes
  {1, {1},       {2},        3,  4}, //   7 nodes
  {1, {1},       {4},        5,  8}, //  15 nodes
  {1, {1},       {8},        9, 16}, //  31 nodes
  {1, {1},       {16},      12, 17}, //  63 nodes
  {3, {3, 5, 9}, {3, 6, 17}, 14, 17}, // 127 nodes
  {3, {5, 9,12}, {5, 9, 17},  1,  0}, // 255 nodes
};

#endif

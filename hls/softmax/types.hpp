#ifndef TYPES_HPP
#define TYPES_HPP

#include <ap_axi_sdata.h>

/*
=============================================
Datatype definitions for INT4 and INT8 inputs
=============================================
*/
// Input stream structure
typedef ap_axis<39,0,0,0> in_axis_t;
typedef ap_fixed<39, 20, AP_RND_CONV, AP_SAT> in_fixed_t;

// Fixed constants
typedef ap_ufixed<32,2> constant_fixed_t;

// Normalized_values / fractional_part math
typedef ap_fixed<42, 22, AP_RND_CONV, AP_SAT> norm_fixed_t;
typedef ap_fixed<42, 22> norm_accum_t;

// Exponentials
typedef ap_ufixed<24, 3, AP_RND_CONV, AP_SAT> exp_fixed_t;
typedef ap_ufixed<32, 5> exp_accum_t;

// Exponential sums
typedef ap_ufixed<48, 16, AP_RND_CONV, AP_SAT> sum_fixed_t;
typedef ap_ufixed<40, 8> sum_accum_t;

// Output stream structure
typedef ap_axis<20,0,0,0> out_axis_t;
typedef ap_ufixed<20, 1, AP_RND_CONV, AP_SAT> out_fixed_t;

/*
=====================================
Datatype definitions for INT16 inputs
=====================================
*/
// typedef ap_axis<37,0,0,0> in_axis_t;
// typedef ap_fixed<37, 18, AP_RND_CONV, AP_SAT> in_fixed_t;
// typedef ap_ufixed<32,2> constant_fixed_t;
// typedef ap_fixed<40, 20, AP_RND_CONV, AP_SAT> norm_fixed_t;
// typedef ap_fixed<40, 20> norm_accum_t;
// typedef ap_ufixed<24, 3, AP_RND_CONV, AP_SAT> exp_fixed_t;
// typedef ap_ufixed<32, 5> exp_accum_t;
// typedef ap_ufixed<48, 16, AP_RND_CONV, AP_SAT> sum_fixed_t;
// typedef ap_ufixed<40, 8> sum_accum_t;
// typedef ap_axis<20,0,0,0> out_axis_t;
// typedef ap_ufixed<20, 1, AP_RND_CONV, AP_SAT> out_fixed_t;

#endif

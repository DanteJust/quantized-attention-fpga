#ifndef TYPES_HPP
#define TYPES_HPP

#include <ap_axi_sdata.h>

// Selected profile
#define PROFILE_INT4

/*
====================================
Datatype definitions for INT4 inputs
====================================
*/
#ifdef PROFILE_INT4
// Input stream structure
typedef ap_axis<20,0,0,0> in_axis_t;
typedef ap_ufixed<20, 1, AP_RND_CONV, AP_SAT> in_fixed_t;

// Items of the input V array
typedef ap_int<4> in_v_t;

// Scale factor
typedef ap_ufixed<20, 0> scale_fixed_t;

// Accumulator
typedef ap_fixed<26, 6> accumulator_fixed_t;

// Output stream structure
typedef ap_axis<20,0,0,0> out_axis_t;
typedef ap_fixed<20, 6, AP_RND_CONV, AP_SAT> out_fixed_t;
#endif

/*
====================================
Datatype definitions for INT8 inputs
====================================
*/
#ifdef PROFILE_INT8
// Input stream structure
typedef ap_axis<20,0,0,0> in_axis_t;
typedef ap_ufixed<20, 1, AP_RND_CONV, AP_SAT> in_fixed_t;

// Items of the input V array
typedef ap_int<8> in_v_t;

// Scale factor
typedef ap_ufixed<20, 0> scale_fixed_t;

// Accumulator
typedef ap_fixed<30, 10> accumulator_fixed_t;

// Output stream structure
typedef ap_axis<20,0,0,0> out_axis_t;
typedef ap_fixed<20, 6, AP_RND_CONV, AP_SAT> out_fixed_t;
#endif

/*
=====================================
Datatype definitions for INT16 inputs
=====================================
*/
#ifdef PROFILE_INT16
// Input stream structure
typedef ap_axis<20,0,0,0> in_axis_t;
typedef ap_ufixed<20, 1, AP_RND_CONV, AP_SAT> in_fixed_t;

// Items of the input V array
typedef ap_int<16> in_v_t;

// Scale factor
typedef ap_ufixed<32, 0> scale_fixed_t;

// Accumulator
typedef ap_fixed<40, 16> accumulator_fixed_t;

// Output stream structure
typedef ap_axis<20,0,0,0> out_axis_t;
typedef ap_fixed<20, 6, AP_RND_CONV, AP_SAT> out_fixed_t;
#endif

#endif

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
typedef ap_axis<24,0,0,0> in_axis_t;
typedef ap_int<24> in_int_t;

// Scale factor
typedef ap_ufixed<32, 0> scale_fixed_t;

// Output stream structure
typedef ap_axis<39,0,0,0> out_axis_t;
typedef ap_fixed<39, 20, AP_RND_CONV, AP_SAT> out_fixed_t;
#endif

/*
====================================
Datatype definitions for INT8 inputs
====================================
*/
#ifdef PROFILE_INT8
// Input stream structure
typedef ap_axis<32,0,0,0> in_axis_t;
typedef ap_int<32> in_int_t;

// Scale factor
typedef ap_ufixed<32, 0> scale_fixed_t;

// Output stream structure
typedef ap_axis<39,0,0,0> out_axis_t;
typedef ap_fixed<39, 20, AP_RND_CONV, AP_SAT> out_fixed_t;
#endif

/*
=====================================
Datatype definitions for INT16 inputs
=====================================
*/
#ifdef PROFILE_INT16
// Input stream structure
typedef ap_axis<46,0,0,0> in_axis_t;
typedef ap_int<46> in_int_t;

// Scale factor
typedef ap_ufixed<46, 0> scale_fixed_t;

// Output stream structure
typedef ap_axis<37,0,0,0> out_axis_t;
typedef ap_fixed<37, 18, AP_RND_CONV, AP_SAT> out_fixed_t;
#endif

#endif

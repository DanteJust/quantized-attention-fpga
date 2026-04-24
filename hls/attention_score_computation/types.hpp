#ifndef TYPES_HPP
#define TYPES_HPP

#include <ap_axi_sdata.h>

/*
====================================
Datatype definitions for INT4 inputs
====================================
*/
// Items of the input Q and K arrays
typedef ap_int<4> in_int_t;

// Output stream structure
typedef ap_axis<24,0,0,0> out_axis_t;
typedef ap_int<24> out_int_t;

/*
====================================
Datatype definitions for INT8 inputs
====================================
*/
// typedef ap_int<8> in_int_t;
// typedef ap_axis<32,0,0,0> out_axis_t;
// typedef ap_int<32> out_int_t;

/*
=====================================
Datatype definitions for INT16 inputs
=====================================
*/
// typedef ap_int<16> in_int_t;
// typedef ap_axis<46,0,0,0> out_axis_t;
// typedef ap_int<46> out_int_t;

#endif

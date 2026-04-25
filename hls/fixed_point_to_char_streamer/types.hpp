#ifndef TYPES_HPP
#define TYPES_HPP

#include "ap_axi_sdata.h"
#include "hls_math.h"

// Input stream structure
typedef ap_axis<20,0,0,0> in_axis_t;
typedef ap_fixed<20, 6> in_fixed_t;

// Output streams stream structure
typedef ap_axis<8,0,0,0> out_axis_t;

#endif

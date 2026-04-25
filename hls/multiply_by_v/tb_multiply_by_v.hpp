#ifndef TB_MULTIPLY_BY_V_HPP
#define TB_MULTIPLY_BY_V_HPP

#include "multiply_by_v.hpp"
#include <hls_math.h>
#include <fstream>

// Constant: Number of rows
#define ROW_COUNT 197

// Constant: Number of elements in each row
#define ROW_ELEMENTS_COUNT 197

// Constant: Head dim -> number of columns in V array
#define HEAD_DIM 64

/*
=========================
Constants for INT4 inputs
=========================
*/
// Constant: V Scale Factor
const scale_fixed_t SCALE_FACTOR = 0.6062701940536499;

/*
=========================
Constants for INT8 inputs
=========================
*/
// const scale_fixed_t SCALE_FACTOR = 0.06156257539987564;

/*
==========================
Constants for INT16 inputs
==========================
*/
// const scale_fixed_t SCALE_FACTOR = 0.00023860734654590487;

#endif

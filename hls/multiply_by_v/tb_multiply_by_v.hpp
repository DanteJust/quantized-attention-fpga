#ifndef TB_MULTIPLY_BY_V_HPP
#define TB_MULTIPLY_BY_V_HPP

#include "multiply_by_v.hpp"
#include <hls_math.h>
#include <fstream>

// Constant: Number of rows
constexpr int ROW_COUNT = 197;

// Constant: Number of elements in each row
constexpr int ROW_ELEMENTS_COUNT = 197;

// Constant: Head dim -> number of columns in V array
constexpr int HEAD_DIM = 64;

/*
=========================
Constants for INT4 inputs
=========================
*/
#ifdef PROFILE_INT4
// Constant: V Scale Factor
const scale_fixed_t SCALE_FACTOR = 0.6062701940536499;
#endif

/*
=========================
Constants for INT8 inputs
=========================
*/
#ifdef PROFILE_INT8
const scale_fixed_t SCALE_FACTOR = 0.06156257539987564;
#endif

/*
==========================
Constants for INT16 inputs
==========================
*/
#ifdef PROFILE_INT16
const scale_fixed_t SCALE_FACTOR = 0.00023860734654590487;
#endif

#endif

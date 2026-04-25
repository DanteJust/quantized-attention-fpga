#ifndef TB_SOFTMAX_HPP
#define TB_SOFTMAX_HPP

#include "softmax.hpp"
#include "constants.hpp"
#include "hls_math.h"
#include <fstream>

// Constant: Number of rows
#define ROW_COUNT 197

// Constant: Number of elements in each row
#define ROW_ELEMENTS_COUNT 197

/*
=========================
Constants for INT4 inputs
=========================
*/
#ifdef PROFILE_INT4
// Constant: Suffix for test files
const std::string filename_suffix = "int4.bin";
#endif

/*
=========================
Constants for INT8 inputs
=========================
*/
#ifdef PROFILE_INT8
const std::string filename_suffix = "int8.bin";
#endif

/*
==========================
Constants for INT16 inputs
==========================
*/
#ifdef PROFILE_INT16
const std::string filename_suffix = "int16.bin";
#endif

#endif

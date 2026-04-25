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
// Constant: Suffix for test files
const std::string filename_suffix = "int4.bin";

/*
=========================
Constants for INT8 inputs
=========================
*/
// const std::string filename_suffix = "int8.bin";

/*
==========================
Constants for INT16 inputs
==========================
*/
// const std::string filename_suffix = "int16.bin";

#endif

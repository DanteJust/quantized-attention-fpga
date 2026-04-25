#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "types.hpp"

// Constant: Number of elements in pre-computed arrays
#define PRECOMPUTED_ELEMENTS_COUNT 14

// Constant: Maximum number of elements in each row of the input stream
#define MAX_ROW_ELEMENTS_COUNT 197

// Constant: Number of iterations for the division loop
#define DIVISION_LOOP_ITERATIONS_COUNT 4

// Constant: Logarithm of e with the base of 2
const constant_fixed_t LOG_2_E = 1.4426950408889634;

// Constant: Natural logarithm of 2
const constant_fixed_t LN_2 = 0.6931471805599453;

// Constant: Array of precomputed values -> 1 + pow(2, -i), where i is the index
const constant_fixed_t Y[PRECOMPUTED_ELEMENTS_COUNT] = {
    2,
    1.5,
    1.25,
    1.125,
    1.0625,
    1.03125,
    1.015625,
    1.0078125,
    1.00390625,
    1.001953125,
    1.0009765625,
    1.00048828125,
    1.000244140625,
    1.0001220703125
};

// Constant: Array of precomputed values -> ln(1 + pow(2, -i)), where i is the index
const constant_fixed_t W[PRECOMPUTED_ELEMENTS_COUNT] = {
    0.6931471805599453,
    0.4054651081081644,
    0.22314355131420976,
    0.11778303565638346,
    0.06062462181643484,
    0.030771658666753687,
    0.015504186535965254,
    0.007782140442054949,
    0.003898640415657323,
    0.0019512201312617493,
    0.0009760859730554589,
    0.0004881620795013512,
    0.0002441108275273627,
    0.00012206286252567737
};

#endif

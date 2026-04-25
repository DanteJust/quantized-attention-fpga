#ifndef TB_SCALER_HPP
#define TB_SCALER_HPP

#include "scaler.hpp"
#include <fstream>

// Constant: Number of elements coming from the input stream
const int NUMBER_OF_ELEMENTS = 197 * 197;

/*
=========================
Constants for INT4 inputs
=========================
*/
#ifdef PROFILE_INT4
// Constant: Combines scale factor (Q_SCALE_FACTOR * K_SCALE_FACTOR / HEAD_DIM_SQRT)
const scale_fixed_t COMBINED_SCALE_FACTOR = 0.0506602317923650041095089080045;
#endif

/*
=========================
Constants for INT8 inputs
=========================
*/
#ifdef PROFILE_INT8
const scale_fixed_t COMBINED_SCALE_FACTOR = 1.5390610230285507744658809156135e-4;
#endif

/*
==========================
Constants for INT16 inputs
==========================
*/
#ifdef PROFILE_INT16
const scale_fixed_t COMBINED_SCALE_FACTOR = 2.3120110905622228802821868966222e-9;
#endif

#endif

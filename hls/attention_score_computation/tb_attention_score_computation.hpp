#ifndef TB_ATTENTION_SCORE_COMPUTATION_HPP
#define TB_ATTENTION_SCORE_COMPUTATION_HPP

#include "attention_score_computation.hpp"
#include <fstream>

// Constant: Sequence length
constexpr int SEQ_LEN = 197;

// Constant: Head dimension
constexpr int HEAD_DIM = 64;

/*
====================================
Datatype definitions for INT4 inputs
====================================
*/
#ifdef PROFILE_INT4
typedef ap_int<32> expected_output_t;
#endif

/*
====================================
Datatype definitions for INT8 inputs
====================================
*/
#ifdef PROFILE_INT8
typedef ap_int<32> expected_output_t;
#endif

/*
=====================================
Datatype definitions for INT16 inputs
=====================================
*/
#ifdef PROFILE_INT16
typedef ap_int<64> expected_output_t;
#endif

#endif

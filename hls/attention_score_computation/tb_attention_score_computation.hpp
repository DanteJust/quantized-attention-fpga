#ifndef TB_ATTENTION_SCORE_COMPUTATION_HPP
#define TB_ATTENTION_SCORE_COMPUTATION_HPP

#include "attention_score_computation.hpp"
#include <fstream>

// Constant: Sequence length
#define SEQ_LEN 197

// Constant: Head dimension
#define HEAD_DIM 64

/*
=============================================
Datatype definitions for INT4 and INT8 inputs
=============================================
*/
typedef ap_int<32> expected_output_t;

/*
=====================================
Datatype definitions for INT16 inputs
=====================================
*/
// typedef ap_int<64> expected_output_t;

#endif

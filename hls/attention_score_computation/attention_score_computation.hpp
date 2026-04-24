#ifndef ATTENTION_SCORE_COMPUTATION_HPP
#define ATTENTION_SCORE_COMPUTATION_HPP

#include "types.hpp"
#include "constants.hpp"

void attention_score_computation(
    const in_int_t Q[MAX_Q_K_LENGTH],
    const in_int_t K[MAX_Q_K_LENGTH],
    hls::stream<out_axis_t> &output_stream,
    int seq_len,
    int head_dim
);

#endif

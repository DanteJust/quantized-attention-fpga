#ifndef MULTIPLY_BY_V_HPP
#define MULTIPLY_BY_V_HPP

#include "types.hpp"
#include "constants.hpp"

void multiply_by_v(
    const in_v_t V[MAX_V_LENGTH],
    hls::stream<in_axis_t> &input_stream,
    hls::stream<out_axis_t> &output_stream,
    int row_elements_count,
    int row_count,
    int head_dim,
    scale_fixed_t v_scale_factor
);

#endif

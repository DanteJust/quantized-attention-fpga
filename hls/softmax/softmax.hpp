#ifndef SOFTMAX_HPP
#define SOFTMAX_HPP

#include <hls_stream.h>
#include "types.hpp"

void preprocess_data(
    hls::stream<in_axis_t> &input_stream,
    hls::stream<in_fixed_t> &output_stream,
    int row_elements_count,
    int row_count
);

static inline exp_fixed_t exp2_approx_from_frac(norm_fixed_t frac_part);

void bit_wise_exponentation(
    hls::stream<in_fixed_t> &input_stream,
    hls::stream<exp_fixed_t> &output_stream,
    hls::stream<sum_fixed_t> &exponentials_sum_stream,
    int row_elements_count,
    int row_count
);

void divide_by_exponentation_sum(
    hls::stream<exp_fixed_t> &input_stream,
    hls::stream<out_axis_t> &output_stream,
    hls::stream<sum_fixed_t> &exponentials_sum_stream,
    int row_elements_count,
    int row_count
);

void softmax(
    hls::stream<in_axis_t> &input_stream,
    hls::stream<out_axis_t> &output_stream,
    int row_count,
    int row_elements_count
);

#endif

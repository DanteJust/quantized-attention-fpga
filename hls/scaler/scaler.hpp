#ifndef SCALER_HPP
#define SCALER_HPP

#include "types.hpp"

void scaler(
    hls::stream<in_axis_t> &input_stream,
    hls::stream<out_axis_t> &output_stream,
    int number_of_elements,
    scale_fixed_t combined_scale_factor
);

#endif

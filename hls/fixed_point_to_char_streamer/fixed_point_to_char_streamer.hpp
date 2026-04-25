#ifndef FIXED_POINT_TO_CHAR_STREAMER_HPP
#define FIXED_POINT_TO_CHAR_STREAMER_HPP

#include "types.hpp"

void fixed_point_to_ascii_converter(in_fixed_t value, unsigned char *out_chars, ap_uint<5> &out_len);
void fixed_point_to_char_streamer(hls::stream<in_axis_t> &input_stream, hls::stream<out_axis_t> &output_stream);

#endif

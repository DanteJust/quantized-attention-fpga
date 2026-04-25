#include "scaler.hpp"

void scaler(
    hls::stream<in_axis_t> &input_stream,
    hls::stream<out_axis_t> &output_stream,
    int number_of_elements,
    scale_fixed_t combined_scale_factor
) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=number_of_elements bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=combined_scale_factor bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    // Loop 1: Iterate over every incoming element
    for (int i = 0; i < number_of_elements; i++) {
        #pragma HLS PIPELINE II=1

        // Read the incoming element
        in_axis_t input_stream_data = input_stream.read();
        in_int_t input_stream_value = input_stream_data.data;

        // Multiply it by the scale factor
        out_fixed_t output_value = input_stream_value * combined_scale_factor;

        // Package the output_value and send it to the output stream
        out_axis_t output_stream_data;
        output_stream_data.data = output_value.range();
        output_stream_data.last = input_stream_data.last;
        output_stream.write(output_stream_data);
    }
}

#include "tb_fixed_point_to_char_streamer.hpp"

int main() {
    // Initialize the input stream and fill it with input data
    hls::stream<in_axis_t> input_stream;
    for (int i = 0; i < 6; i++) {
        in_axis_t input_stream_data;
        input_stream_data.data = input_data[i].range();
        input_stream_data.last = i == 5;
        input_stream.write(input_stream_data);
    }

    // Initialize the output stream
    hls::stream<out_axis_t> output_stream;

    // Call the function
    fixed_point_to_char_streamer(input_stream, output_stream);

    // Validate the results
    while (!output_stream.empty()) {
        std::cout << (char)(output_stream.read().data);
    }

    return 0;
}

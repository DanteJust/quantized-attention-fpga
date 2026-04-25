#include "tb_softmax.hpp"

template <typename T>
void read_binary(const std::string &filename, std::vector<T> &data) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }
    ifs.read(reinterpret_cast<char*>(data.data()), data.size() * sizeof(T));
    ifs.close();
}

int main() {
    // Prepare vectors for the input data and expected output
    std::vector<double> inputs(ROW_COUNT * ROW_ELEMENTS_COUNT);
    std::vector<double> expected_output(ROW_COUNT * ROW_ELEMENTS_COUNT);

    // Populate vectors
    read_binary<double>("testbench_data/inputs_" + filename_suffix, inputs);
    read_binary<double>("testbench_data/outputs_" + filename_suffix, expected_output);

    // Prepare the input stream and fill it with data
    hls::stream<in_axis_t> input_stream;
    for (int i = 0; i < ROW_COUNT * ROW_ELEMENTS_COUNT; i++) {
        in_axis_t input_data;
        input_data.data = in_fixed_t(inputs[i]).range();
        input_data.last = i == ROW_COUNT * ROW_ELEMENTS_COUNT - 1;
        input_stream.write(input_data);
    }

    // Initialize the output stream and call the softmax function
    hls::stream<out_axis_t> output_stream;
    softmax(input_stream, output_stream, ROW_COUNT, ROW_ELEMENTS_COUNT);

    // Compare the outputs
    const double TOLERANCE = 1e-5;
    int failed_items = 0;
    int i = 0;
    while (!output_stream.empty()) {
        out_fixed_t output_stream_data;
        output_stream_data.range() = output_stream.read().data;
        if (hls::fabs((double)output_stream_data - expected_output[i]) > TOLERANCE) failed_items++;
        i++;
    }

    // Print the result
    if (failed_items == 0) {
        std::cout << "Test has passed!" << std::endl;
        return 0;
    }
    std::cout << "Test has failed with: " << failed_items << " items." << std::endl;
    return 1;
}

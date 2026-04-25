#include "tb_scaler.hpp"

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
    std::vector<in_int_t> inputs(NUMBER_OF_ELEMENTS);
    std::vector<double> expected_output(NUMBER_OF_ELEMENTS);

    // Resolve filename suffix based on the input datatype
    std::string filename_suffix;
    switch (in_int_t::width) {
        case 24:
            filename_suffix = "int4.bin";
            break;
        case 46:
            filename_suffix = "int16.bin";
            break;
        default:
            filename_suffix = "int8.bin";
    }

    // Populate the vectors
    if (in_int_t::width == 24 || in_int_t::width == 46) {
        // Inputs are stored in 32-bit or 64-bit format
        using temporary_input_type = std::conditional_t<in_int_t::width == 24, ap_int<32>, ap_int<64>>;

        // Prepare the vector
        std::vector<temporary_input_type> temporary_inputs(NUMBER_OF_ELEMENTS);

        // Read the input
        read_binary<temporary_input_type>("testbench_data/inputs_" + filename_suffix, temporary_inputs);

        // Cast down the data
        for (int i = 0; i < temporary_inputs.size(); i++) inputs[i] = in_int_t(temporary_inputs[i]);
    } else {
        read_binary<in_int_t>("testbench_data/inputs_" + filename_suffix, inputs);
    }

    // Populate the expected outputs vector
    read_binary<double>("testbench_data/outputs_" + filename_suffix, expected_output);

    // Initialize the input stream and fill it with data
    hls::stream<in_axis_t> input_stream;
    for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {
        in_axis_t input_data;
        input_data.data = inputs[i];
        input_data.last = i == NUMBER_OF_ELEMENTS - 1;
        input_stream.write(input_data);
    }

    // Initialize the output stream and call the function
    hls::stream<out_axis_t> output_stream;
    scaler(input_stream, output_stream, NUMBER_OF_ELEMENTS, COMBINED_SCALE_FACTOR);

    // Compare the outputs
    const double TOLERANCE = 1e-5; // 0.00001 is sufficient precision
    int failed_items = 0;
    int i = 0;
    while (!output_stream.empty()) {
        out_axis_t output_data = output_stream.read();
        ap_int<out_fixed_t::width> raw_bits = output_data.data;
        out_fixed_t actual_fixed_value;
        actual_fixed_value.range() = raw_bits;
        double actual_double_value = double(actual_fixed_value);
        if (std::fabs(actual_double_value - expected_output[i]) > TOLERANCE) failed_items++;
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

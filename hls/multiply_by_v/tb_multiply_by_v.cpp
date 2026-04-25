#include "tb_multiply_by_v.hpp"

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
    // Prepare vectors for the input data, expected output and V tensor
    std::vector<double> inputs(ROW_COUNT * ROW_ELEMENTS_COUNT);
    std::vector<double> outputs(ROW_COUNT * HEAD_DIM);
    std::vector<in_v_t> V(ROW_ELEMENTS_COUNT * HEAD_DIM);

    // Resolve filename suffix based on the datatype
    std::string filename_suffix;
    switch (in_v_t::width) {
        case 4:
            filename_suffix = "int4";
            break;
        case 16:
            filename_suffix = "int16";
            break;
        default:
            filename_suffix = "int8";
    }

    // Populate V vector
    if (in_v_t::width == 4) {
        // INT4 inputs are stored in INT8 format with empty bits at the end
        typedef ap_int<8> temporary_input_type;

        // Prepare the temporary vector
        std::vector<temporary_input_type> V_temporary(ROW_ELEMENTS_COUNT * HEAD_DIM);

        // Populate it
        read_binary<temporary_input_type>("testbench_data/V_" + filename_suffix + "_head.bin", V_temporary);

        // Inputs are INT4 nonetheless, so just cast them down
        for (int i = 0; i < V_temporary.size(); i++) V[i] = in_v_t(V_temporary[i]);
    } else {
        read_binary<in_v_t>("testbench_data/V_" + filename_suffix + "_head.bin", V);
    }

    // Populate inputs and outputs vectors
    read_binary<double>("testbench_data/inputs_" + filename_suffix + ".bin", inputs);
    read_binary<double>("testbench_data/outputs_" + filename_suffix + ".bin", outputs);

    // Prepare the input stream and fill it with data
    hls::stream<in_axis_t> input_stream;
    for (int i = 0; i < ROW_COUNT * ROW_ELEMENTS_COUNT; i++) {
        in_axis_t input_data;
        input_data.data = in_fixed_t(inputs[i]).range();
        input_data.last = i == ROW_COUNT * ROW_ELEMENTS_COUNT - 1;
        input_stream.write(input_data);
    }

    // Initialize the output stream and call the function
    hls::stream<out_axis_t> output_stream;
    multiply_by_v(V.data(), input_stream, output_stream, ROW_ELEMENTS_COUNT, ROW_COUNT, HEAD_DIM, SCALE_FACTOR);

    // Compare the results
    const double TOLERANCE = 1e-4;
    int failed_items = 0;
    int i = 0;
    while (!output_stream.empty()) {
        out_fixed_t received_value;
        received_value.range() = output_stream.read().data;
        if (hls::fabs((double)received_value - outputs[i]) > TOLERANCE) failed_items++;
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

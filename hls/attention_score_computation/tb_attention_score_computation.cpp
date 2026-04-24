#include "tb_attention_score_computation.hpp"

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
    // Initialize the vectors
    std::vector<in_int_t> Q(SEQ_LEN * HEAD_DIM);
    std::vector<in_int_t> K(SEQ_LEN * HEAD_DIM);
    std::vector<expected_output_t> expected_output(SEQ_LEN * SEQ_LEN);

    // Resolve filename suffix based on the datatype
    std::string filename_suffix;
    switch (in_int_t::width) {
        case 4:
            filename_suffix = "int4.bin";
            break;
        case 16:
            filename_suffix = "int16.bin";
            break;
        default:
            filename_suffix = "int8.bin";
    }

    // Populate the vectors
    if (in_int_t::width == 4) {
        // INT4 inputs are stored in INT8 format with empty bits at the end
        typedef ap_int<8> temporary_input_type;

        // Prepare the vectors
        std::vector<temporary_input_type> Q_int8(SEQ_LEN * HEAD_DIM);
        std::vector<temporary_input_type> K_int8(SEQ_LEN * HEAD_DIM);

        // Read the inputs
        read_binary<temporary_input_type>("testbench_data/Q_" + filename_suffix, Q_int8);
        read_binary<temporary_input_type>("testbench_data/K_" + filename_suffix, K_int8);

        // Inputs are INT4 nonetheless, so just cast them down
        for (int i = 0; i < Q_int8.size(); i++) {
            Q[i] = in_int_t(Q_int8[i]);
            K[i] = in_int_t(K_int8[i]);
        }
    } else {
        read_binary<in_int_t>("testbench_data/Q_" + filename_suffix, Q);
        read_binary<in_int_t>("testbench_data/K_" + filename_suffix, K);
    }

    // Populate the expected outputs
    read_binary<expected_output_t>("testbench_data/attention_score_" + filename_suffix, expected_output);

    // Prepare the output stream and call the function
    hls::stream<out_axis_t> output_stream;
    attention_score_computation(Q.data(), K.data(), output_stream, SEQ_LEN, HEAD_DIM);

    // Compare outputs
    int failed_items = 0;
    int i = 0;
    while (!output_stream.empty()) {
        out_axis_t stream_value = output_stream.read();
        expected_output_t received_value = expected_output_t(stream_value.data);
        expected_output_t expected_value = expected_output[i];
        if (received_value != expected_value) failed_items++;
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

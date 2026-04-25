#include "multiply_by_v.hpp"

void multiply_by_v(
    const in_v_t V[MAX_V_LENGTH],
    hls::stream<in_axis_t> &input_stream,
    hls::stream<out_axis_t> &output_stream,
    int row_elements_count,
    int row_count,
    int head_dim,
    scale_fixed_t v_scale_factor
) {
    #pragma HLS INTERFACE bram port=V storage_type=rom_2p
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=row_elements_count bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=row_count bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=head_dim bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=v_scale_factor bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    // Prepare array where we will store all the inputs of the target row
    in_fixed_t row_input_items[MAX_ROW_ELEMENTS_COUNT];

    // Loop 1: Loop over each row of the input elements
    for (int i = 0; i < row_count; i++) {

        // Loop 2: Read all inputs from the row and store them locally
        for (int x = 0; x < row_elements_count; x++) {
            #pragma HLS PIPELINE II=1

            row_input_items[x].range() = input_stream.read().data;
        }

        // Loop 3: Iterate over columns of V
        for (int z = 0; z < head_dim; z += 2) {
            #pragma HLS PIPELINE II=1

            accumulator_fixed_t sum_0 = 0;
            accumulator_fixed_t sum_1 = 0;
            bool has_second = (z + 1) < head_dim;
            
            // Loop 4: Perform the dot product
            for (int d = 0; d < row_elements_count; d++) {
                #pragma HLS PIPELINE II=1

                sum_0 += row_input_items[d] * V[d * head_dim + z];
                if (has_second) sum_1 += row_input_items[d] * V[d * head_dim + (z + 1)];
            }

            // Process and write the first sum
            out_fixed_t sum_scaled_0 = sum_0 * v_scale_factor;
            out_axis_t output_stream_data_0;
            output_stream_data_0.data = sum_scaled_0.range();
            output_stream_data_0.last = (i == row_count - 1) && (z == head_dim - 2 && head_dim % 2 == 0);
            output_stream.write(output_stream_data_0);

            // Process and write the second sum
            if (has_second) {
                out_fixed_t sum_scaled_1 = sum_1 * v_scale_factor;
                out_axis_t output_stream_data_1;
                output_stream_data_1.data = sum_scaled_1.range();
                output_stream_data_1.last = (i == row_count - 1) && (z + 1 == head_dim - 1);
                output_stream.write(output_stream_data_1);
            }
        }

    }
}

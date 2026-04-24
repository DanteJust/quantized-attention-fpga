#include "attention_score_computation.hpp"

void attention_score_computation(
    const in_int_t Q[MAX_Q_K_LENGTH],
    const in_int_t K[MAX_Q_K_LENGTH],
    hls::stream<out_axis_t> &output_stream,
    int seq_len,
    int head_dim
) {
    #pragma HLS INTERFACE bram port=Q storage_type=rom_2p
    #pragma HLS INTERFACE bram port=K storage_type=rom_2p
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=seq_len bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=head_dim bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    // Loop 1: Loop over rows of Q matrix
    for (int i = 0; i < seq_len; i++) {

        // Loop 2: Loop over rows of K matrix (basically columns of transposed K matrix)
        for (int j = 0; j < seq_len; j++) {

            // Initialize the partial sum variables
            out_int_t sum_0 = 0;
            out_int_t sum_1 = 0;

            // Loop 3: Perform the dot product by utilizing 2 port ROM access
            for (int d = 0; d < head_dim; d += 2) {
                #pragma HLS PIPELINE II=1

                // Calculate corresponding indexes of Q and K arrays
                int q_idx_0 = i * head_dim + d;
                int q_idx_1 = i * head_dim + d + 1;
                int k_idx_0 = j * head_dim + d;
                int k_idx_1 = j * head_dim + d + 1;

                // Update the first partial sum
                out_int_t prod_0 = Q[q_idx_0] * K[k_idx_0];
                sum_0 += prod_0;

                // Update the second partial sum only if d + 1 is smaller than head dim
                out_int_t prod_1 = (d + 1 < head_dim) ? out_int_t(Q[q_idx_1] * K[k_idx_1]) : out_int_t(0);
                sum_1 += prod_1;
            }

            // Send the dot product in the output stream
            out_axis_t value;
            value.data = sum_0 + sum_1;
            value.last = (i == seq_len - 1) && (j == seq_len - 1);
            output_stream.write(value);
        }
    }
}

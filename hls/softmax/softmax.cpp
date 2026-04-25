#include "softmax.hpp"
#include "constants.hpp"

void preprocess_data(
    hls::stream<in_axis_t> &input_stream,
    hls::stream<in_fixed_t> &output_stream,
    int row_elements_count,
    int row_count
) {
    // Loop 1: Loop over each row of the input stream
    for (int i = 0; i < row_count; i++) {
        // Initialize local buffer for the row elements
        in_fixed_t input_processed[MAX_ROW_ELEMENTS_COUNT];

        // Initialize row max value and flag that states whether it has been already initialized
        in_fixed_t max_value;
        bool is_max_value_initialized = false;

        // Loop 2: Read and store all input elements to the local buffer, and also find the max value
        for (int x = 0; x < row_elements_count; x++) {
            #pragma HLS PIPELINE II=1

            input_processed[x].range() = input_stream.read().data;
            if (!is_max_value_initialized || input_processed[x] > max_value) {
                max_value = input_processed[x];
                is_max_value_initialized = true;
            }
        }

        // Convert max value to integer to lose the fractional part
        ap_int<in_fixed_t::iwidth> integer_max_value = max_value.to_ap_int();

        // Loop 3: Subtract every element by the integer_max_value to normalize it and send the final value to the output stream
        for (int x = 0; x < row_elements_count; x++) {
            #pragma HLS PIPELINE II=1

            in_fixed_t final_value = input_processed[x] - integer_max_value;
            output_stream.write(final_value);
        }
    }
}

static inline exp_fixed_t exp2_approx_from_frac(norm_fixed_t frac_part) {
    // Map fractional part to core_input in [0, LN2)
    constant_fixed_t core_input = frac_part * LN_2;

    // Keep multiplication chain in wider accumulator
    exp_accum_t result = 1;

    for (int y = 0; y < PRECOMPUTED_ELEMENTS_COUNT; y++) {
        #pragma HLS UNROLL
        if (core_input < W[y]) continue;
        core_input -= W[y];
        result *= Y[y];
    }

    // Cast to exp_fixed_t before returning
    return exp_fixed_t(result);
}

void bit_wise_exponentation(
    hls::stream<in_fixed_t> &input_stream,
    hls::stream<exp_fixed_t> &output_stream,
    hls::stream<sum_fixed_t> &exponentials_sum_stream,
    int row_elements_count,
    int row_count
) {
    // Initialize the row exponentials sum
    sum_fixed_t exponentials_row_sum;

    // Loop 1: Loop over each row
    for (int i = 0; i < row_count; i++) {

        // Set sum to 0
        exponentials_row_sum = 0;

        // Loop 2: Loop over every element of the target row
        for (int x = 0; x < row_elements_count; x++) {
            #pragma HLS PIPELINE II=1

            // Multiply the input value by the logarithm to normalize
            norm_fixed_t normalized_value = input_stream.read() * LOG_2_E;

            // Split the value to integer and fractional part
            ap_int<norm_fixed_t::iwidth> integer_part = normalized_value.to_ap_int();
            if (normalized_value < 0) integer_part -= 1;
            norm_fixed_t fractional_part = normalized_value - integer_part;

            // Approximate 2^(fractional_part) in positive domain
            exp_fixed_t frac_exp2 = exp2_approx_from_frac(fractional_part);

            // Apply 2^(integer_part) scaling safely for signed integer_part
            exp_accum_t scaled = frac_exp2;
            if (integer_part >= 0) {
                scaled = scaled << integer_part;
            } else {
                scaled = scaled >> (-integer_part);
            }

            // Send the result to the output stream and append it to the row exponentials sum
            exp_fixed_t final_result = exp_fixed_t(scaled);
            output_stream.write(final_result);
            exponentials_row_sum += sum_accum_t(final_result);

            // On last iteration, send the sum through a stream
            if (x == row_elements_count - 1) exponentials_sum_stream.write(sum_fixed_t(exponentials_row_sum));
        }
    }
}

void divide_by_exponentation_sum(
    hls::stream<exp_fixed_t> &input_stream,
    hls::stream<out_axis_t> &output_stream,
    hls::stream<sum_fixed_t> &exponentials_sum_stream,
    int row_elements_count,
    int row_count
) {
    sum_fixed_t reciprocal_value;
    int total_iterations = row_count * row_elements_count;
    int current_column_index = 0;

    // Loop 1: Loop over each item
    for (int i = 0; i < total_iterations; i++) {
        #pragma HLS PIPELINE II=1

        // Upon hitting first element in the row, calculate the reciprocal value for the target row
        if (current_column_index == 0) {
            sum_fixed_t exponentials_row_sum = exponentials_sum_stream.read();

            // LOD: Find position of the most significant bit in the row sum
            ap_int<sum_fixed_t::iwidth> row_sum_integer_part = exponentials_row_sum.to_ap_int();
            ap_uint<6> LOD = (sum_fixed_t::iwidth - 1) - row_sum_integer_part.countLeadingZeros();

            // Normalize the input by doing a right shift by Sa = LOD + 1
            sum_fixed_t normalized_row_sum = exponentials_row_sum >> (LOD + 1);

            // Initialize the N to 1 and denominator to normalized_row_sum
            sum_accum_t N = 1;
            sum_accum_t denominator = normalized_row_sum;

            // Loop 2: Calculate the final value of N
            for (int x = 0; x < DIVISION_LOOP_ITERATIONS_COUNT; x++) {
                #pragma HLS UNROLL

                sum_accum_t R = 2 - denominator;
                denominator *= R;
                N *= R;
            }

            // Calculate the reciprocal value by doing the right shift by Sa
            reciprocal_value = N >> (LOD + 1);
        }

        // Calculate the result of the division
        out_fixed_t calculated_value = input_stream.read() * reciprocal_value;
        out_axis_t output_stream_data;
        output_stream_data.data = calculated_value.range();
        output_stream_data.last = i == total_iterations - 1;
        output_stream.write(output_stream_data);

        // Manage internal counter
        current_column_index = (current_column_index == row_elements_count - 1) ? 0 : current_column_index + 1;
    }
}

void softmax(
    hls::stream<in_axis_t> &input_stream,
    hls::stream<out_axis_t> &output_stream,
    int row_count,
    int row_elements_count
) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=row_count bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=row_elements_count bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL
    #pragma HLS DATAFLOW

    // Initialize all internal streams
    hls::stream<in_fixed_t> preprocessed_data_stream;
    #pragma HLS STREAM variable=preprocessed_data_stream depth=256
    hls::stream<exp_fixed_t> exponentation_values_stream;
    #pragma HLS STREAM variable=exponentation_values_stream depth=256
    hls::stream<sum_fixed_t> exponentials_sum_stream;
    #pragma HLS STREAM variable=exponentials_sum_stream depth=32

    // Start all functions in paralell
    preprocess_data(input_stream, preprocessed_data_stream, row_elements_count, row_count);
    bit_wise_exponentation(preprocessed_data_stream, exponentation_values_stream, exponentials_sum_stream, row_elements_count, row_count);
    divide_by_exponentation_sum(exponentation_values_stream, output_stream, exponentials_sum_stream, row_elements_count, row_count);
}

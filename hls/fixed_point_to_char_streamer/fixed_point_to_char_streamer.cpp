#include "fixed_point_to_char_streamer.hpp"

void fixed_point_to_ascii_converter(in_fixed_t value,
                                    unsigned char *out_chars,
                                    ap_uint<5> &out_len) {
    const int DECIMAL_PLACES = 8;
    const int POWER_OF_10    = 100000000; // 10^8

    in_fixed_t abs_value = hls::abs(value);
    bool is_negative = (value < 0);

    int idx = 0;
    unsigned char temp[30];
    int t = 0;

    // Use a wider integer type so we don't overflow on larger inputs
    ap_int<32> integer_part = abs_value.to_ap_int();

    // Fractional part in [0, 1)
    in_fixed_t fractional_part = abs_value - (in_fixed_t)integer_part;

    // Convert fractional part to integer with rounding, and use a wide type
    // Needs to hold up to 99,999,999 (27 bits), so use 32 bits.
    ap_uint<32> frac_as_int = (ap_uint<32>)((fractional_part * POWER_OF_10) + (in_fixed_t)0.5);

    // Handle rounding carry: 0.999999995 -> 1.00000000
    if (frac_as_int >= (ap_uint<32>)POWER_OF_10) {
        frac_as_int -= (ap_uint<32>)POWER_OF_10;
        integer_part += 1;
    }

    // Fractional digits (least significant first)
    for (int i = 0; i < DECIMAL_PLACES; ++i) {
        temp[t++] = (unsigned char)('0' + (frac_as_int % 10));
        frac_as_int /= 10;
    }

    // Decimal point
    temp[t++] = '.';

    // Integer digits (least significant first)
    if (integer_part == 0) {
        temp[t++] = '0';
    } else {
        while (integer_part > 0) {
            temp[t++] = (unsigned char)('0' + (integer_part % 10));
            integer_part /= 10;
        }
    }

    // Sign
    if (is_negative) temp[t++] = '-';

    // Reverse into output
    for (int i = 0; i < t; i++) out_chars[idx++] = temp[t - 1 - i];

    out_chars[idx] = '\0';
    out_len = idx;
}

void fixed_point_to_char_streamer(hls::stream<in_axis_t> &input_stream, hls::stream<out_axis_t> &output_stream) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE ap_ctrl_none port=return

    while (1) {
        // If the stream is empty, do not continue
        if (input_stream.empty()) continue;

        // Read the input stream
        in_axis_t input_stream_value = input_stream.read();
        in_fixed_t input_stream_data;
        input_stream_data.range() = input_stream_value.data;

        // Prepare the variables needed for conversion and convert the data
        unsigned char out[30];
        ap_uint<5> out_len;
        fixed_point_to_ascii_converter(input_stream_data, out, out_len);

        // Pass the data to the next stream
        for (int i = 0; i < out_len; i++) {
            out_axis_t output_stream_data;
            output_stream_data.data = out[i];
            output_stream_data.last = 0;
            output_stream.write(output_stream_data);
        }

        // Add CR and LF characters for proper new line
        out_axis_t cr, lf;
        cr.data = '\r';
        cr.last = 0;
        output_stream.write(cr);
        lf.data = '\n';
        lf.last = 1;
        output_stream.write(lf);

        // Exit the loop if the last signal was received
        if (input_stream_value.last) break;
    }
}

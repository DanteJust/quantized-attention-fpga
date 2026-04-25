#include "xparameters.h"
#include "constants_int4.hpp"
#include "xattention_head_qk.h"
#include "xqk_scaler.h"
#include "xsoftmax.h"
#include "xmultiply_by_v.h"

static inline uint64_t double_to_q0_46_bits(double x)
{
    const uint64_t Q0_46_MAX = (1ULL << 46) - 1ULL;   // 0x3FFFFFFFFFFF
    const double   TWO_POW_46 = 70368744177664.0;     // 2^46

    if (x <= 0.0) return 0ULL;
    if (x >= 1.0) return Q0_46_MAX;                  // clamp

    double scaled = x * TWO_POW_46;
    uint64_t v = (uint64_t)(scaled + 0.5);           // round to nearest

    if (v > Q0_46_MAX) v = Q0_46_MAX;
    return v; // only lower 46 bits are used
}

static inline uint32_t double_to_q0_32(double x)
{
    if (x <= 0.0f) return 0u;
    if (x >= 1.0f) return 0xFFFFFFFFu; // clamp (since 1.0 can't be represented in Q0.32)

    // scale by 2^32 using double for accuracy
    double scaled = x * 4294967296.0; // 2^32
    uint64_t v = (uint64_t)(scaled + 0.5);    // round to nearest

    if (v > 0xFFFFFFFFull) v = 0xFFFFFFFFull;
    return (uint32_t)v;
}

static inline uint32_t double_to_q0_20(double x)
{
    if (x <= 0.0f) return 0u;
    if (x >= 1.0f) return (1u << 20) - 1;  // 0x000FFFFF

    double scaled = x * (double)(1u << 20);
    uint32_t v = (uint32_t)(scaled + 0.5); // round to nearest

    if (v > ((1u << 20) - 1))
        v = (1u << 20) - 1;

    return v;
}

int main() {
    // Initialize Attention QK IP and set it's arguments
    XAttention_head_qk qk_ip;
    XAttention_head_qk_Initialize(&qk_ip, 0);
    XAttention_head_qk_Set_head_dim(&qk_ip, HEAD_DIM);
    XAttention_head_qk_Set_seq_len(&qk_ip, SEQ_LEN);

    // Initialize QK Scaler IP and set it's arguments
    XQk_scaler qk_scaler_ip;
    XQk_scaler_Initialize(&qk_scaler_ip, 0);
    XQk_scaler_Set_number_of_elements(&qk_scaler_ip, SEQ_LEN * SEQ_LEN);
    XQk_scaler_Set_combined_scale_factor(&qk_scaler_ip, double_to_q0_32(COMBINED_SCALE_FACTOR)); // Use for INT4 and INT8
    // XQk_scaler_Set_combined_scale_factor(&qk_scaler_ip, double_to_q0_46_bits(COMBINED_SCALE_FACTOR)); // Use for INT16

    // Initialize Softmax IP and set it's arguments
    XSoftmax softmax_ip;
    XSoftmax_Initialize(&softmax_ip, 0);
    XSoftmax_Set_row_count(&softmax_ip, SEQ_LEN);
    XSoftmax_Set_row_elements_count(&softmax_ip, SEQ_LEN);

    // Initialize Multiply by V IP and set it's arguments
    XMultiply_by_v multiply_by_v_ip;
    XMultiply_by_v_Initialize(&multiply_by_v_ip, 0);
    XMultiply_by_v_Set_head_dim(&multiply_by_v_ip, HEAD_DIM);
    XMultiply_by_v_Set_row_count(&multiply_by_v_ip, SEQ_LEN);
    XMultiply_by_v_Set_row_elements_count(&multiply_by_v_ip, SEQ_LEN);
    XMultiply_by_v_Set_v_scale_factor(&multiply_by_v_ip, double_to_q0_20(V_SCALE_FACTOR)); // Use for INT4 and INT8
    // XMultiply_by_v_Set_v_scale_factor(&multiply_by_v_ip, double_to_q0_32(V_SCALE_FACTOR)); // Use for INT16

    // Start all the IPs
    XAttention_head_qk_Start(&qk_ip);
    XQk_scaler_Start(&qk_scaler_ip);
    XSoftmax_Start(&softmax_ip);
    XMultiply_by_v_Start(&multiply_by_v_ip);

    // Wait until all IPs have finished their work
    while (!XAttention_head_qk_IsDone(&qk_ip) || !XQk_scaler_IsDone(&qk_scaler_ip) || !XSoftmax_IsDone(&softmax_ip) || !XMultiply_by_v_IsDone(&multiply_by_v_ip));

    return 0;
}

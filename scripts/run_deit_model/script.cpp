#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <sstream>

// ------------------- FILE LOADER ------------------- //

std::vector<float> load_bin(const std::string& filename, size_t num_elements) {
    std::vector<float> data(num_elements);
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        exit(1);
    }
    infile.read(reinterpret_cast<char*>(data.data()), num_elements * sizeof(float));
    
    // Check if we read the expected amount
    if (infile.gcount() != num_elements * sizeof(float)) {
        std::cerr << "Error: Expected " << num_elements << " elements from " << filename 
                  << ", but got " << infile.gcount() / sizeof(float) << std::endl;
        exit(1);
    }
    
    infile.close();
    return data;
}

// ------------------- Functions for Embeddings ------------------- //

inline float get_weight(const float* W, int out_c, int in_c, int ph, int pw, 
                       int out_channels=192, int in_channels=3, int patch_h=16, int patch_w=16) {
    return W[out_c*in_channels*patch_h*patch_w + in_c*patch_h*patch_w + ph*patch_w + pw];
}

void patch_embed(const float* image, const float* W, const float* b, std::vector<float>& out) {
    const int C=3, H=224, W_img=224;
    const int patch_size=16;
    const int num_patches_h = H/patch_size;
    const int num_patches_w = W_img/patch_size;
    const int embed_dim = 192;

    out.resize(num_patches_h*num_patches_w*embed_dim, 0.0f);

    for(int ph=0; ph<num_patches_h; ph++) {
        for(int pw=0; pw<num_patches_w; pw++) {
            int patch_idx = ph*num_patches_w + pw;
            for(int oc=0; oc<embed_dim; oc++) {
                float sum = b[oc];
                for(int c=0; c<C; c++) {
                    for(int kh=0; kh<patch_size; kh++) {
                        for(int kw=0; kw<patch_size; kw++) {
                            int h_idx = ph*patch_size + kh;
                            int w_idx = pw*patch_size + kw;
                            float pixel = image[c*H*W_img + h_idx*W_img + w_idx];
                            sum += pixel * get_weight(W, oc, c, kh, kw);
                        }
                    }
                }
                out[patch_idx*embed_dim + oc] = sum;
            }
        }
    }
}

void add_cls_pos(const std::vector<float>& patch_tokens,
                 const float* cls_token,
                 const float* pos_embed,
                 std::vector<float>& output) {
    const int num_patches = 196;
    const int embed_dim = 192;

    output.resize((num_patches+1)*embed_dim);

    // CLS token + pos embedding
    for(int d=0; d<embed_dim; d++)
        output[d] = cls_token[d] + pos_embed[d];

    // Patch tokens + pos embedding
    for(int t=0; t<num_patches; t++) {
        for(int d=0; d<embed_dim; d++) {
            output[(t+1)*embed_dim + d] = patch_tokens[t*embed_dim + d] + pos_embed[(t+1)*embed_dim + d];
        }
    }
}

// ------------------- Encoder ------------------- //

void layernorm(const float* input, float* output, int seq_len, int hidden_dim, 
               const float* gamma, const float* beta, float eps=1e-6f) {
    for(int t=0; t<seq_len; t++) {
        const float* x = &input[t*hidden_dim];
        float* y = &output[t*hidden_dim];
        
        // Compute mean
        float mean = 0.0f;
        for(int d=0; d<hidden_dim; d++) mean += x[d];
        mean /= hidden_dim;

        // Compute variance
        float var = 0.0f;
        for(int d=0; d<hidden_dim; d++) {
            float tmp = x[d] - mean;
            var += tmp*tmp;
        }
        var /= hidden_dim;
        float denom = 1.0f / std::sqrt(var + eps);

        // Normalize and apply gamma/beta
        for(int d=0; d<hidden_dim; d++)
            y[d] = (x[d] - mean) * denom * gamma[d] + beta[d];
    }
}

void linear(const float* input, const float* W, const float* b,
            float* output, int in_dim, int out_dim) {
    for(int o=0; o<out_dim; o++) {
        float sum = b[o];
        for(int i=0; i<in_dim; i++) {
            sum += input[i] * W[o*in_dim + i];
        }
        output[o] = sum;
    }
}

void softmax(float* v, int len) {
    float max_val = v[0];
    for(int i=1; i<len; i++) {
        if(v[i] > max_val) max_val = v[i];
    }

    float sum = 0.0f;
    for(int i=0; i<len; i++){
        v[i] = std::exp(v[i] - max_val);
        sum += v[i];
    }
    
    if (sum > 0.0f) {
        for(int i=0; i<len; i++) v[i] /= sum;
    }
}

// More accurate GELU implementation
float gelu(float x) {
    return 0.5f * x * (1.0f + std::tanh(std::sqrt(2.0f / M_PI) * (x + 0.044715f * x * x * x)));
}

void multi_head_attention(const std::vector<float>& x,
                          int layer,
                          const float* qkv_W, const float* qkv_b,
                          const float* out_W, const float* out_b,
                          std::vector<float>& out,
                          int seq_len=197, int hidden_dim=192, int num_heads=3) {
    int head_dim = hidden_dim / num_heads;
    std::vector<float> qkv(seq_len * 3 * hidden_dim);
    std::vector<float> attn_out(seq_len * hidden_dim, 0.0f);

    // 1. Compute QKV projection
    for(int t=0; t<seq_len; t++) {
        linear(&x[t*hidden_dim], qkv_W, qkv_b, &qkv[t*3*hidden_dim], hidden_dim, 3*hidden_dim);
    }

    // Uncomment this if you want to store the QKV projection to binary file:
    // std::cout << "Storing QKV projection of encoder layer " << layer << " to a binary file." << std::endl;
    // std::ofstream fout("qkv_" + std::to_string(layer) + ".bin", std::ios::binary);
	// if (fout) {
    //     fout.write(reinterpret_cast<const char*>(qkv.data()), qkv.size() * sizeof(float));
    //     fout.close();
    //     std::cout << "QKV written to qkv_" << layer << ".bin (" << qkv.size() * sizeof(float) << " bytes)" << std::endl;
	// }

    // 2. Compute attention per head
    for(int h=0; h<num_heads; h++) {
        int h_offset = h * head_dim;
        
        // Compute attention scores
        std::vector<float> scores(seq_len * seq_len, 0.0f);
        for(int i=0; i<seq_len; i++) {
            int q_offset = i * 3 * hidden_dim + h_offset;
            for(int j=0; j<seq_len; j++) {
                int k_offset = j * 3 * hidden_dim + hidden_dim + h_offset;
                float score = 0.0f;
                for(int d=0; d<head_dim; d++) {
                    score += qkv[q_offset + d] * qkv[k_offset + d];
                }
                scores[i*seq_len + j] = score / std::sqrt(static_cast<float>(head_dim));
            }
        }

        // Apply softmax to each row
        for(int i=0; i<seq_len; i++) {
            softmax(&scores[i*seq_len], seq_len);
        }

        // Apply attention to values
        for(int i=0; i<seq_len; i++) {
            int out_offset = i * hidden_dim + h_offset;
            for(int d=0; d<head_dim; d++) {
                float val = 0.0f;
                for(int j=0; j<seq_len; j++) {
                    int v_offset = j * 3 * hidden_dim + 2*hidden_dim + h_offset + d;
                    val += scores[i*seq_len + j] * qkv[v_offset];
                }
                attn_out[out_offset + d] = val;
            }
        }
    }

    // 3. Output projection
    out.resize(seq_len * hidden_dim);
    for(int t=0; t<seq_len; t++) {
        linear(&attn_out[t*hidden_dim], out_W, out_b, &out[t*hidden_dim], hidden_dim, hidden_dim);
    }
}

void mlp(const std::vector<float>& x, const float* fc1_W, const float* fc1_b,
         const float* fc2_W, const float* fc2_b, std::vector<float>& out,
         int seq_len=197, int hidden_dim=192, int mlp_hidden=768) {
    out.resize(seq_len * hidden_dim);
    std::vector<float> hidden(mlp_hidden);

    for(int t=0; t<seq_len; t++) {
        // First linear layer + GELU
        linear(&x[t*hidden_dim], fc1_W, fc1_b, hidden.data(), hidden_dim, mlp_hidden);
        for(int i=0; i<mlp_hidden; i++) {
            hidden[i] = gelu(hidden[i]);
        }
        
        // Second linear layer
        linear(hidden.data(), fc2_W, fc2_b, &out[t*hidden_dim], mlp_hidden, hidden_dim);
    }
}

void encoder_block(std::vector<float>& x,
                   int layer,
                   const float* qkv_W, const float* qkv_b,
                   const float* attn_out_W, const float* attn_out_b,
                   const float* gamma1, const float* beta1,
                   const float* fc1_W, const float* fc1_b,
                   const float* fc2_W, const float* fc2_b,
                   const float* gamma2, const float* beta2,
                   int seq_len=197, int hidden_dim=192, int mlp_hidden=768) {
    
    std::vector<float> ln1_out(seq_len * hidden_dim);
    std::vector<float> attn_out(seq_len * hidden_dim);
    std::vector<float> residual1(seq_len * hidden_dim);
    std::vector<float> ln2_out(seq_len * hidden_dim);
    std::vector<float> mlp_out(seq_len * hidden_dim);
    
    // 1. LayerNorm before attention
    layernorm(x.data(), ln1_out.data(), seq_len, hidden_dim, gamma1, beta1);
    
    // 2. Multi-head attention
    multi_head_attention(ln1_out, layer, qkv_W, qkv_b, attn_out_W, attn_out_b, attn_out, seq_len, hidden_dim);

    // 3. First residual connection
    for(int i=0; i<seq_len*hidden_dim; i++) {
        residual1[i] = x[i] + attn_out[i];
    }
    
    // 4. LayerNorm before MLP
    layernorm(residual1.data(), ln2_out.data(), seq_len, hidden_dim, gamma2, beta2);
    
    // 5. MLP
    mlp(ln2_out, fc1_W, fc1_b, fc2_W, fc2_b, mlp_out, seq_len, hidden_dim, mlp_hidden);
    
    // 6. Second residual connection
    for(int i=0; i<seq_len*hidden_dim; i++) {
        x[i] = residual1[i] + mlp_out[i];
    }
}

// Fixed QKV concatenation function
std::vector<float> concat_qkv(const std::vector<float>& q,
                              const std::vector<float>& k,
                              const std::vector<float>& v) {
    size_t dim = q.size();
    if (k.size() != dim || v.size() != dim) {
        std::cerr << "Error: Q, K, V must have same dimension" << std::endl;
        exit(1);
    }
    
    std::vector<float> qkv;
    qkv.reserve(3 * dim);
    
    // Concatenate in Q-K-V order
    qkv.insert(qkv.end(), q.begin(), q.end());
    qkv.insert(qkv.end(), k.begin(), k.end());
    qkv.insert(qkv.end(), v.begin(), v.end());
    
    return qkv;
}

std::string construct_filename(const std::string& prefix, int layer, const std::string& postfix) {
    std::stringstream ss;
    ss << prefix << layer << postfix;
    return ss.str();
}

void run_encoder(std::vector<float>& x, int layer) {
    // Load weights
    std::vector<float> q_W = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_attention_attention_query_weight.bin"), 192 * 192);
    std::vector<float> k_W = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_attention_attention_key_weight.bin"), 192 * 192);
    std::vector<float> v_W = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_attention_attention_value_weight.bin"), 192 * 192);
    
    std::vector<float> q_B = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_attention_attention_query_bias.bin"), 192);
    std::vector<float> k_B = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_attention_attention_key_bias.bin"), 192);
    std::vector<float> v_B = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_attention_attention_value_bias.bin"), 192);
    
    // Concatenate QKV weights and biases
    std::vector<float> qkv_W = concat_qkv(q_W, k_W, v_W);
    std::vector<float> qkv_B = concat_qkv(q_B, k_B, v_B);
    
    std::vector<float> attn_out_W = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_attention_output_dense_weight.bin"), 192 * 192);
    std::vector<float> attn_out_B = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_attention_output_dense_bias.bin"), 192);
    
    std::vector<float> gamma1 = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_layernorm_before_weight.bin"), 192);
    std::vector<float> beta1 = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_layernorm_before_bias.bin"), 192);
    
    std::vector<float> fc1_W = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_intermediate_dense_weight.bin"), 768 * 192);
    std::vector<float> fc1_B = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_intermediate_dense_bias.bin"), 768);
    
    std::vector<float> fc2_W = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_output_dense_weight.bin"), 192 * 768);
    std::vector<float> fc2_B = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_output_dense_bias.bin"), 192);
    
    std::vector<float> gamma2 = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_layernorm_after_weight.bin"), 192);
    std::vector<float> beta2 = load_bin(construct_filename("weights/vit_encoder_layer_", layer, "_layernorm_after_bias.bin"), 192);
    
    encoder_block(x, layer, qkv_W.data(), qkv_B.data(), attn_out_W.data(), attn_out_B.data(), 
                  gamma1.data(), beta1.data(), fc1_W.data(), fc1_B.data(), 
                  fc2_W.data(), fc2_B.data(), gamma2.data(), beta2.data());
}

void softmax_on_logits(const std::vector<float>& logits, std::vector<float>& probs) {
    int n = logits.size();
    probs.resize(n);

    // 1. subtract max for numerical stability
    float max_val = *std::max_element(logits.begin(), logits.end());
    float sum = 0.0f;
    for(int i=0;i<n;i++){
        probs[i] = std::exp(logits[i]-max_val);
        sum += probs[i];
    }

    // 2. normalize
    for(int i=0;i<n;i++) probs[i] /= sum;
}

// ------------------- Main Function ------------------- //


int main() {
    // Load the input image
    const int C = 3, H = 224, W = 224;
    size_t image_size = C * H * W;
    std::vector<float> image = load_bin("input_image.bin", image_size);
    std::cout << "Loaded image with " << image.size() << " floats." << std::endl;

    // Load the embeddings
    std::vector<float> cls_token = load_bin("weights/vit_embeddings_cls_token.bin", 192);
    std::vector<float> position_embeddings = load_bin("weights/vit_embeddings_position_embeddings.bin", 197 * 192);
    std::vector<float> projection_weight = load_bin("weights/vit_embeddings_patch_embeddings_projection_weight.bin", 192 * 3 * 16 * 16);
    std::vector<float> projection_bias = load_bin("weights/vit_embeddings_patch_embeddings_projection_bias.bin", 192);

    // Step 1: Patch embedding
    std::vector<float> patch_tokens;
    patch_embed(image.data(), projection_weight.data(), projection_bias.data(), patch_tokens);

    // Step 2: Add CLS + positional embeddings
    std::vector<float> vit_embeddings;
    add_cls_pos(patch_tokens, cls_token.data(), position_embeddings.data(), vit_embeddings);

    std::cout << "Embeddings computed, size: " << vit_embeddings.size() << std::endl;

    // Step 3: Run encoder layers
    for (int layer = 0; layer < 12; layer++) {
        run_encoder(vit_embeddings, layer);
        std::cout << "Encoder layer " << layer << " completed." << std::endl;
    }

    // Step 4: Extract cls token and apply final layernorm
    std::vector<float> final_cls_token(192);
    for(int d = 0; d < 192; d++) {
        final_cls_token[d] = vit_embeddings[d];
    }

    std::vector<float> layernorm_weight = load_bin("weights/vit_layernorm_weight.bin", 192);
    std::vector<float> layernorm_bias = load_bin("weights/vit_layernorm_bias.bin", 192);
    
    std::vector<float> normalized_cls(192);
    layernorm(final_cls_token.data(), normalized_cls.data(), 1, 192, layernorm_weight.data(), layernorm_bias.data());

    // Step 5: Apply classifier
    std::vector<float> classifier_weight = load_bin("weights/classifier_weight.bin", 1000 * 192);
    std::vector<float> classifier_bias = load_bin("weights/classifier_bias.bin", 1000);
    
    std::vector<float> logits(1000);
    linear(normalized_cls.data(), classifier_weight.data(), classifier_bias.data(), logits.data(), 192, 1000);

    // Step 6: Softmax and prediction
    std::vector<float> probs(1000);
    softmax_on_logits(logits, probs);

    int predicted_class = std::max_element(probs.begin(), probs.end()) - probs.begin();
    float max_prob = probs[predicted_class];

    std::cout << "Predicted class: " << predicted_class << " with probability " << max_prob * 100 << std::endl;

    return 0;
}

#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    int seq_len = 197;
    int head_dim = 64;

    // Read the files and store the values
    std::vector<float> Q_head(seq_len * head_dim);
    std::vector<float> K_head(seq_len * head_dim);
    std::vector<float> V_head(seq_len * head_dim);

    std::ifstream fiq("Q_head.bin", std::ios::binary);
    if (!fiq) {
        std::cerr << "Error opening file for reading!\n";
        return 1;
    }
    fiq.read(reinterpret_cast<char*>(Q_head.data()), Q_head.size() * sizeof(float));
    fiq.close();

    std::ifstream fik("K_head.bin", std::ios::binary);
    if (!fik) {
        std::cerr << "Error opening file for reading!\n";
        return 1;
    }
    fik.read(reinterpret_cast<char*>(K_head.data()), K_head.size() * sizeof(float));
    fik.close();

    std::ifstream fiv("V_head.bin", std::ios::binary);
    if (!fiv) {
        std::cerr << "Error opening file for reading!\n";
        return 1;
    }
    fiv.read(reinterpret_cast<char*>(V_head.data()), V_head.size() * sizeof(float));
    fiv.close();

    // Declare attention scores matrix
    std::vector<double> scores(seq_len * seq_len, 0.0);

    // Initialize output of attention
    std::vector<double> attn_out(seq_len * head_dim, 0.0);

    // Compute dot products
    for (int i = 0; i < seq_len; i++) {       // loop over queries
        for (int j = 0; j < seq_len; j++) {   // loop over keys
            double score = 0.0;
            for (int d = 0; d < head_dim; d++) {
                score += Q_head[i * head_dim + d] * K_head[j * head_dim + d];
            }
            // scale
            score /= sqrtf(head_dim);

            scores[i * seq_len + j] = score; // store in intermediate matrix
        }
    }

    // Apply softmax per query
    for (int i = 0; i < seq_len; i++) {
        double max_val = -1e9;
        for (int j = 0; j < seq_len; j++)
            if (scores[i * seq_len + j] > max_val) max_val = scores[i * seq_len + j];

        double sum = 0.0;
        for (int j = 0; j < seq_len; j++) {
            scores[i * seq_len + j] = expf(scores[i * seq_len + j] - max_val);
            sum += scores[i * seq_len + j];
        }
        for (int j = 0; j < seq_len; j++)
            scores[i * seq_len + j] /= sum;
    }

    // Multiply by V to get output
    for (int i = 0; i < seq_len; i++) {           // loop over queries
        for (int d = 0; d < head_dim; d++) {      // loop over head dimensions
            double val = 0.0;
            for (int j = 0; j < seq_len; j++) {   // sum over keys/values
                val += scores[i * seq_len + j] * V_head[j * head_dim + d];
            }
            attn_out[i * head_dim + d] = val;
        }
    }

    // Save the output to a file
    std::ofstream output_file("attention_outputs.txt");
    if (!output_file) {
        std::cerr << "Error opening file for writing!\n";
        return 1;
    }
    output_file << std::fixed << std::setprecision(8);
    for (const double& value : attn_out) {
        output_file << value;
        output_file << "\n";
    }
    output_file.close();
    std::cout << "Successfully wrote " << attn_out.size() << " numbers" << std::endl;

    return 0;
}

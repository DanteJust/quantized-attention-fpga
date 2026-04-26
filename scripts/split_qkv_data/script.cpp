#include <vector>
#include <fstream>
#include <iostream>

int main() {
	// Define the sequence length and hidden dim
	const int seq_len = 197;
	const int hidden_dim = 192;

	// Define the QKV vector and load data into it
	std::vector<float> qkv(seq_len * 3 * hidden_dim);
	
	std::ifstream fin("qkv.bin", std::ios::binary);
	if (!fin) {
		std::cerr << "Error opening file for reading!\n";
		return 1;
	}
	fin.read(reinterpret_cast<char*>(qkv.data()), qkv.size() * sizeof(float));
	fin.close();

	// Now we need to choose a head and split the QKV for that head into three different files (Q, K, V)
	int num_heads = 3;
	int head_dim = hidden_dim / num_heads;
	int h = 0; // Choose the first head
	int h_offset = h * head_dim;
	int token_stride = 3 * hidden_dim;

	std::vector<float> Q_head(seq_len * head_dim);
	std::vector<float> K_head(seq_len * head_dim);
	std::vector<float> V_head(seq_len * head_dim);

	for (int t = 0; t < seq_len; t++) {
		int base = t * token_stride;
		for (int d = 0; d < head_dim; d++) {
			int index = t * head_dim + d;
			Q_head[index] = qkv[base + h_offset + d];
			K_head[index] = qkv[base + hidden_dim + h_offset + d];
			V_head[index] = qkv[base + 2 * hidden_dim + h_offset + d];
		}
	}

	std::ofstream fq("Q_head.bin", std::ios::binary);
	fq.write(reinterpret_cast<const char*>(Q_head.data()), Q_head.size() * sizeof(float));
	fq.close();

	std::ofstream fk("K_head.bin", std::ios::binary);
	fk.write(reinterpret_cast<const char*>(K_head.data()), K_head.size() * sizeof(float));
	fk.close();

	std::ofstream fv("V_head.bin", std::ios::binary);
	fv.write(reinterpret_cast<const char*>(V_head.data()), V_head.size() * sizeof(float));
	fv.close();
}

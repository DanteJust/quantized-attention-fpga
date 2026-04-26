## Calculate Attention Head

This script computes a single attention head and writes the results to `attention_outputs.txt`. It requires the binary input files `K_head.bin`, `Q_head.bin`, and `V_head.bin`, all stored in full precision (`FP32`).

In the paper, this script is used to generate CPU reference values.

To compile the script run:
```bash
c++ script.cpp -o script
```

Then execute it with:
```bash
./script
```

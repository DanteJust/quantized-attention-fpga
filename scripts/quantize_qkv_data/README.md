## Quantization of Q, K and V tensors
This script quantizes binary files to a lower precision. By default, the input files should be of precision `FP32` and called `Q_head.bin`, `K_head.bin`, and `V_head.bin`. On the other hand, the output precision can be set during the script execution.

### Environment Preparation
To prepare the environment, run the following commands:
```bash
python3 -m venv .venv
source ./.venv/bin/activate
pip install numpy
```

### Script Execution
To execute the script, simply run:
```bash
python script.py
```

### Script Outputs
The output consists of:
- `Q_int8_head.bin` -> Binary file consisting of quantized Q values.
- `K_int8_head.bin` -> Binary file consisting of quantized K values.
- `V_int8_head.bin` -> Binary file consisting of quantized V values.
- `scales_head.npz` -> NumPy multi-array consisting of scales for each quantized file (used to dequantize values).

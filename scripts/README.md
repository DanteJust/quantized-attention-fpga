## Scripts
This folder contains all scripts used in the paper’s evaluation. Running them is not strictly required, since all intermediate datasets are already provided in the `/app` and `/vivado` directories. However, to fully reproduce the entire data preparation pipeline, the process is as follows.

### Data and Weight Extraction
The raw data is located in `prepare_weights_or_image_data/data/`. Before use, it must be normalized and converted into a binary format. A script in this directory is used to preprocess the input image and extract the weights for the DeIT-Tiny model.

### Extracting QKV Data
Since the evaluation focuses on a single attention head, only the corresponding QKV data is required. A script in `/run_deit_model` runs the full model and includes a commented section (lines 165–172) that saves each QKV projection to a binary file. The script can be stopped after the first projection is written.

### Splitting the Data
After extraction, all QKV values are stored in a single binary file. This file must be split into components: `Q_head.bin`, `K_head.bin`, and `V_head.bin`. The script in `split_qkv_data/` performs this operation.

### CPU Calculation
Once the data is split, an optional reference computation can be performed using the script in `calculate_attention_head/`. This produces CPU-based reference outputs.

### Quantization
The QKV data can then be quantized to the target precision using the script located in `quantize_qkv_data/`.

### Converting Data to COE Format
After quantization, the data must be converted into a format compatible with Vivado. This is done using the script in `transform_bin_to_coe/`, which generates `.coe` files from the binary inputs.

### Saving Output to TXT
Although not part of the data preparation pipeline, an additional script is provided to read output data via UART during execution on the Basys3 board and store it in a `.txt` file for analysis.

## Split QKV Data into Separate Binary Files for a Single Head
This script splits full QKV data (generated for all attention heads) into separate binary files for a single selected head. The target head is specified inside the script.

### Prerequisites
- The input QKV file must exist and be named `qkv.bin`.
- The sequence length and hidden dimension must be set in advance.
- These values must exactly match the settings used when generating the original QKV data.

### Running the Script
First, make sure the `qkv.bin` file is present. Then compile the script:
```bash
c++ script.cpp -o script
```

After compilation, run it:
```bash
./script
```

The script will generate three output files:
- `Q_head.bin`
- `K_head.bin`
- `V_head.bin`

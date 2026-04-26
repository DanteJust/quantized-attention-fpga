## Run DeIT Model
This script runs DeIT model but only if appropriate files are provided:
- `input_image.bin` -> This is the corresponding input file that will be classified.
- `weights/` -> Directory containing all weights for the DeIT model.

To run the script just first compile it:
```bash
gcc main.cpp -o transformer
```

And then run it:
```bash
./transformer
```

### Usage in the Paper
This script is used in the paper to extract the `qkv.bin` file. It contains commented code on lines 165–172 that saves the projection output to a binary file. Since the paper targets only a single attention head, the script can be stopped after the first projection has been written to the file.

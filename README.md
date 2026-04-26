# Quantized Attention Head on a Resouce-Constrained FPGA
This repository contains all the code required to reproduce the results presented in the paper “Area–Accuracy Evaluation of a Quantized Attention Head on a Resource-Constrained FPGA”.

## Project Structure
The project is organized as follows:
```
|- app          # Vitis application that runs on the Basys3 board
|- docs         # Images used in README files
|- hls          # IP blocks implemented using Vitis HLS
|- scripts      # Scripts used for evaluation in the paper
|- vivado       # Vivado project files
|- README.md
```

## Quick Run
To quickly run the project:
1. Connect the Basys3 board to your host computed.
2. Run the application located in the `/app` directory.

No data preprocessing is required, since the `/app` directory already contains all necessary inputs for execution.

To collect output data form the board, use the script located at:
```
/scripts/read_store_serial_data/script.py
```

## Full (Long) Run
To execute the full workflow:

1. Prepare the data using the scripts in the `/scripts` directory. A tutorial is included there explaining the required steps.
2. Open and run the corresponding Vivado project.
3. Generate the `.xsa` hardware export file.
4. Use this `.xsa` file to create a Vitis platform.
5. Build the Vitis application provided in the `/app` directory using the generated platform.
6. Run the application on the Basys3 board.
7. Collect the output data using the `read_store_serial_data` script.

## Prerequisites
For running the Python scripts, it is recommended to use Python version 3.10 or newer, since newer language features are used throughout the project.

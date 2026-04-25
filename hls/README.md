## Vitis HLS Components

To compile the IP, launch Vitis and open this directory. Then navigate to the desired IP block, open the `hls_config.cfg` file through the project settings, and follow these steps:

1. In the C Synthesis Sources section, select the main source file (for example, `attention_score_computation.cpp`), then click the Browse icon and reselect the same file. This updates Vitis with the full Windows file path, which is required for the tool to detect the available functions in the next step.
2. Choose the corresponding top function.
3. Run the following steps in order: C Simulation, C Synthesis, and finally Package.
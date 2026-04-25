## Running the application

To run the application, follow these steps:

1. Open Vitis and create a new platform using one of the provided `.xsas` files.
2. After the platform is created, build it.
3. Create a new Vitis application and select the generated platform as its base.
4. Add the appropriate header file (`constants_int4.hpp`, `constants_int8.hpp`, or `constants_int16.hpp`) depending on the precision level of the selected `.xsas` platform.
5. In the chosen header file, set the target profile according to the selected `.xsas configuration` (for example: #define PROFILE_CAT1).
6. Create a new `main.cpp` file and ensure it includes the correct constants header. By default, `constants_int4.hpp` is used, but this may need to be changed depending on the selected `.xsas` platform.
7. If INT16 is used as the base precision for the `.xsas` platform, modify the code accordingly: comment out line 61, uncomment line 62, comment out line 76, and uncomment line 77 so that the implementation is compatible with the INT16 configuration. For INT4 and INT8 configurations, the default version can be used without modification.
8. Build the application.
9. Run the application (ensure the Basys3 board is connected to the PC).
10. The results are streamed over the UART interface. You can use the provided script to capture the output and save it to a `.txt` file.
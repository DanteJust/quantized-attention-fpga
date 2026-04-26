import sys
import numpy as np

input_bin = input("Input binary file: ")
output_coe = input("Output coe name: ")

print("1. INT16")
print("2. INT8")
print("3. INT4")
operation_num = int(input("Please select the precision level of the input binary file: "))

match operation_num:
    case 1:
        values = np.fromfile(input_bin, dtype=np.int16)
        formatter = lambda x: f"{np.uint16(x):04X}"
    case 2:
        values = np.fromfile(input_bin, dtype=np.int8)
        formatter = lambda x: f"{np.uint8(x):02X}"
    case 3:
        values = np.fromfile(input_bin, dtype=np.int8)
        formatter = lambda x: f"{(int(x) & 0xF):X}"
    case _:
        print("Invalid precision level.")
        sys.exit(1)

with open(output_coe, "w") as f:
    f.write("memory_initialization_radix = 16;\n")
    f.write("memory_initialization_vector =\n")

    for i, v in enumerate(values):
        hex_val = formatter(v)
        sep = ",\n" if i < len(values) - 1 else ";\n"
        f.write(f"{hex_val}{sep}")

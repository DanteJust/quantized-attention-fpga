import sys
import argparse
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("--precision")
parser.add_argument("--input_file")
args = parser.parse_args()

if args.precision == "INT16":
    values = np.fromfile(args.input_file, dtype=np.int16)
    formatter = lambda x: f"{np.uint16(x):04X}"
elif args.precision == "INT8":
    values = np.fromfile(args.input_file, dtype=np.int8)
    formatter = lambda x: f"{np.uint8(x):02X}"
elif args.precision == "INT4":
    values = np.fromfile(args.input_file, dtype=np.int8)
    formatter = lambda x: f"{(int(x) & 0xF):X}"
else:
    print("Invalid precision level")
    sys.exit(1)

with open(args.input_file.replace(".bin", ".coe"), "w") as f:
    f.write("memory_initialization_radix = 16;\n")
    f.write("memory_initialization_vector =\n")

    for i, v in enumerate(values):
        hex_val = formatter(v)
        sep = ",\n" if i < len(values) - 1 else ";\n"
        f.write(f"{hex_val}{sep}")


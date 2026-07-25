#!/bin/bash

# Parse Arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        --image)
            IMAGE="$2"
            shift 2
            ;;
        *)
            echo "Unknown argument: $1"
            exit 1
            ;;
    esac
done

if [[ -z "$IMAGE" ]]; then
	echo "Error: --image is required"
	exit 1
fi

# Compile all C++ scripts
g++ run_deit_model/script.cpp -o run_deit_model/transformer
g++ split_qkv_data/script.cpp -o split_qkv_data/split
echo "All C++ scripts have been compiled"

# Run all scripts
cd prepare_weights_or_image_data && python script.py --action save_weights && python script.py --action save_image --image "$IMAGE" && cd ..
mv prepare_weights_or_image_data/weights run_deit_model/
mv prepare_weights_or_image_data/input_image.bin run_deit_model/
cd run_deit_model && ./transformer && cd ..
mv run_deit_model/qkv_0.bin split_qkv_data/qkv.bin
cd split_qkv_data && ./split && cd ..
mv split_qkv_data/Q_head.bin split_qkv_data/K_head.bin split_qkv_data/V_head.bin quantize_qkv_data/
cd quantize_qkv_data && python script.py --precision INT4 --percentile 99.0 && python script.py --precision INT8 --percentile 100.0 && python script.py --precision INT16 --percentile 100.0 && cd ..
mv quantize_qkv_data/Q_head_INT4.bin quantize_qkv_data/K_head_INT4.bin quantize_qkv_data/V_head_INT4.bin quantize_qkv_data/Q_head_INT8.bin quantize_qkv_data/K_head_INT8.bin quantize_qkv_data/V_head_INT8.bin quantize_qkv_data/Q_head_INT16.bin quantize_qkv_data/K_head_INT16.bin quantize_qkv_data/V_head_INT16.bin transform_bin_to_coe/
cd transform_bin_to_coe
for precision_level in "INT16" "INT8" "INT4"; do
	python script.py --precision "$precision_level" --input_file "Q_head_${precision_level}.bin"
	python script.py --precision "$precision_level" --input_file "K_head_${precision_level}.bin"
	python script.py --precision "$precision_level" --input_file "V_head_${precision_level}.bin"
done
cd ..
echo "All scripts have successfully ran"

# Cleanup all files
rm -r run_deit_model/transformer run_deit_model/input_image.bin run_deit_model/weights split_qkv_data/split split_qkv_data/qkv.bin
for i in {1..11}; do
	rm "run_deit_model/qkv_${i}.bin"
done
rm quantize_qkv_data/Q_head.bin quantize_qkv_data/K_head.bin quantize_qkv_data/V_head.bin
for precision_level in "INT16" "INT8" "INT4"; do
	rm "transform_bin_to_coe/Q_head_${precision_level}.bin" "transform_bin_to_coe/K_head_${precision_level}.bin" "transform_bin_to_coe/V_head_${precision_level}.bin"
done
echo "All files created during the run have been cleaned up"


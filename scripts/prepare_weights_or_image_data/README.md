## Prepare Image Data or Network Weights
This script supports two functions:
1. Converts input images into the binary representation required for the evaluation pipeline.
2. Extracts the pretrained DeIT-Tiny network weights and stores them as binary files.

### Environment Setup
Create and activate a Python virtual environment, then install the required dependencies:
```bash
python3 -m venv .venv
source ./.venv/bin/activate
pip install torch transformers pillow numpy
```

### Running the Script
Execute the script with:
```bash
python script.py
```

### Input Image Sources
The evaluation uses six publicly available images selected to represent three semantic categories: cat, boxer-type dogs, and retriever-type dogs (two samples per category).

#### Cat
- Cat 1: https://huggingface.co/datasets/huggingface/documentation-images/resolve/main/cats.png
- Cat 2: https://kittentoob.com/wp-content/uploads/2018/01/Mackerel-tabby-750x441.jpg

#### Boxer
- Boxer 1: https://huggingface.co/Sena/dog/resolve/main/images/buldog.jpg
- Boxer 2: https://raw.githubusercontent.com/EliSchwartz/imagenet-sample-images/refs/heads/master/n02108089_boxer.JPEG

#### Retriever
- Retriever 1: https://huggingface.co/Sena/dog/resolve/main/images/golden.jpg
- Retriever 2: https://raw.githubusercontent.com/EliSchwartz/imagenet-sample-images/refs/heads/master/n02099601_golden_retriever.JPEG

These images are used solely as representative evaluation inputs for reproducing the experiments described in the paper.

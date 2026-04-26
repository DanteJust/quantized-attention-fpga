import os
import numpy as np
from transformers import AutoModelForImageClassification, PreTrainedModel
from PIL import Image


def load_model(model_name: str) -> PreTrainedModel:
    """
    Loads image classification model by its name.

    :param model_name: Name of the target model.
    :returns: Pre-trained target model.
    """
    return AutoModelForImageClassification.from_pretrained(model_name)


def save_image(image_path: str, binary_file_path: str) -> None:
    """
    Loads target image, normalizes it for the ImageNet and stores it to a binary file.

    :param image_path: Path of the locally stored image to load.
    :param binary_file_path: Path for the newly created binary file.
    :returns: None.
    """
    image = Image.open(image_path).convert("RGB")
    image = image.resize((224,224))
    # Convert to numpy [H,W,C] → [C,H,W]
    arr = np.array(image).astype(np.float32) / 255.0
    arr = arr.transpose(2,0,1)
    # Apply ImageNet normalization
    mean = np.array([0.485, 0.456, 0.406], dtype=np.float32).reshape(3,1,1)
    std  = np.array([0.229, 0.224, 0.225], dtype=np.float32).reshape(3,1,1)
    arr = (arr - mean) / std
    arr.tofile(binary_file_path)
    print(f"Image saved ({arr.size} floats)")


def save_weights(model: PreTrainedModel) -> None:
    """
    Stores weights for all layers of the target model to binary files.

    :param model: Model whose weights will be stored.
    :returns: None.
    """
    model.eval()
    os.makedirs("weights", exist_ok=True)
    for name, param in model.state_dict().items():
        # Flatten weights into 1D array
        arr = param.cpu().numpy().flatten()
        # Replace dots with underscores for filename
        fname = f"weights/{name.replace('.', '_')}.bin"
        arr.tofile(fname)
        print(f"Saved {fname} ({arr.size} floats)")


def parse_integer(input_value: str) -> int:
    """
    Parses the input value to a integer, if it's not possible, it exists the application.

    :param input_value: Value to parse.
    :returns: Parsed integer.
    """
    try:
        return int(input_value)
    except ValueError:
        print("Could not parse the input value to integer.")
        exit(1)


if __name__ == "__main__":
    model_name = "facebook/deit-tiny-patch16-224"
    print("1. Load and save the image to a binary file.")
    print("2. Save the model's weights to binary files.")
    operation_num = parse_integer(input("Please select the operation to perform: "))
    match operation_num:
        case 1:
            print("1. Cat 1")
            print("2. Cat 2")
            print("3. Boxer 1")
            print("4. Boxer 2")
            print("5. Retriever 1")
            print("6. Retriever 2")
            target_image_num = parse_integer(input("Please select an image: "))
            match target_image_num:
                case 1:
                    image_path = "data/cat_1.png"
                case 2:
                    image_path = "data/cat_2.png"
                case 3:
                    image_path = "data/boxer_1.jpg"
                case 4:
                    image_path = "data/boxer_2.jpeg"
                case 5:
                    image_path = "data/retriever_1.jpg"
                case 6:
                    image_path = "data/retriever_2.jpeg"
                case _:
                    print("Incorrect image selected.")
                    exit(1)
            binary_file_path = input("Please select output file path: ")
            save_image(image_path, binary_file_path)
        case 2:
            model = load_model(model_name)
            save_weights(model)
        case _:
            print("Unknown operation.")

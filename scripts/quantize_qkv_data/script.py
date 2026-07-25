import numpy as np
import sys
import argparse


def read_qkv_data(q_path: str, k_path: str, v_path: str) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Reads the unquantized QKV data for one attention head from input files and
    returns them in form of numpy ndarrays.

    :param q_path: Path to the binary file containing Q data.
    :param k_path: Path to the binary file containing K data.
    :param v_path: Path to the binary file contianing V data.
    :returns: A tuple of numpy arrays in this format -> [Q, K, V].
    """
    Q = np.fromfile(q_path, dtype=np.float32)
    K = np.fromfile(k_path, dtype=np.float32)
    V = np.fromfile(v_path, dtype=np.float32)
    return Q, K, V


def store_quantized_qkv_data(q_path: str, k_path: str, v_path: str, Q: np.ndarray, K: np.ndarray, V: np.ndarray) -> None:
    """
    Writes quantized matrices to the output files.

    :param q_path: Path for the Q binary file.
    :param k_path: Path for the K binary file.
    :param v_path: Path for the V binary file.
    :param Q: Numpy array holding Q data.
    :param K: Numpy array holding K data.
    :param V: Numpy array holding V data.
    :returns: None.
    """
    Q.tofile(q_path)
    K.tofile(k_path)
    V.tofile(v_path)


def store_scales(q_scale: np.generic, k_scale: np.generic, v_scale: np.generic, file_name: str) -> None:
    """
    Writes scales to the common numpy zip file.

    :param q_scale: Scalar representing Q scale.
    :param k_scale: Scalar representing K scale.
    :param v_scale: Scalar representing V scale.
    :param file_name: Name of the target file.
    :returns: None.
    """
    np.savez(file_name, sQ=q_scale, sK=k_scale, sV=v_scale)


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
        sys.exit(1)


def quantize(data: np.ndarray, precision: str, percentile: float) -> tuple[np.ndarray, np.generic]:
    """
    Transforms input data to INT16/INT8/INT4 representation.
    :param data: Data to transform.
    :param percentile: Percentile used for outlier clipping.
    :returns: Tuple of transformed data and applied scale.
    """
    if precision not in ["INT4", "INT8", "INT16"]:
        raise Exception("Invalid precision selected.")
    if precision == "INT4":
        qmin, qmax = -8, 7
    elif precision == "INT8":
        qmin, qmax = -128, 127
    else:
        qmin, qmax = -32768, 32767
    target_np_type = np.int16 if precision == "INT16" else np.int8
    clip = np.percentile(np.abs(data), percentile)
    scale = clip / qmax + 1e-12
    data_q = np.clip(np.round(data / scale), qmin, qmax).astype(target_np_type)
    return data_q, scale


if __name__ == "__main__":
    ## Setup the argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument("--precision")
    parser.add_argument("--percentile")
    args = parser.parse_args()
    if not args.precision or not args.percentile:
        raise Exception("--precision and --percentile arguments are required")

    ## Read the data
    Q, K, V = read_qkv_data(q_path='Q_head.bin', k_path='K_head.bin', v_path='V_head.bin')

    ## Quantize
    Q_q, sQ = quantize(Q, args.precision, float(args.percentile))
    K_q, sK = quantize(K, args.precision, float(args.percentile))
    V_q, sV = quantize(V, args.precision, float(args.percentile))

    ## Store inputs and weights
    store_quantized_qkv_data(f"Q_head_{args.precision}.bin", f"K_head_{args.precision}.bin", f"V_head_{args.precision}.bin", Q_q, K_q, V_q)
    store_scales(sQ, sK, sV, f"scales_head_{args.precision}.npz")
    print("Data has been saved.")


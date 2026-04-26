import numpy as np


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


def quantize_to_int16(data: np.ndarray) -> tuple[np.ndarray, np.generic]:
    """
    Transforms input data to INT16 representation.

    :param data: Data to transform.
    :returns: Tuple of transformed data and applied scale.
    """
    qmin = -32768
    qmax = 32767
    scale = np.max(np.abs(data)) / qmax
    data_q = np.round(data / scale)
    data_q = np.clip(data_q, qmin, qmax).astype(np.int16)
    return data_q, scale


def quantize_to_int8(data: np.ndarray) -> tuple[np.ndarray, np.generic]:
    """
    Transforms input data to INT8 representation.

    :param data: Data to transform.
    :returns: Tuple of transformed data and applied scale.
    """
    qmax = 127
    scale = np.max(np.abs(data)) / qmax
    data_q = np.round(data / scale).astype(np.int8)
    return data_q, scale


def quantize_to_int4(data: np.ndarray) -> tuple[np.ndarray, np.generic]:
    """
    Transforms input data to INT4 representaion.

    :param data: Data to transform.
    :returns: Tuple of transformed data and applied scale.
    """
    picked_percentile = 99.0
    qmin, qmax = -8, 7
    clip = np.percentile(np.abs(data), picked_percentile)
    scale = clip / qmax + 1e-12
    data_q = np.clip(np.round(data / scale), qmin, qmax).astype(np.int8)
    return data_q, scale


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
    ## Read the data
    Q, K, V = read_qkv_data(q_path='Q_head.bin', k_path='K_head.bin', v_path='V_head.bin')

    ## Select desired quantization precision
    print("1. Quantize to INT16.")
    print("2. Quantize to INT8.")
    print("3. Quantize to INT4.")
    operation_num = parse_integer(input("Please select the quantization precision: "))

    ## Quantize
    match operation_num:
        case 1:
            Q_q, sQ = quantize_to_int16(Q)
            K_q, sK = quantize_to_int16(K)
            V_q, sV = quantize_to_int16(V)
            suffix = "int16.bin"
        case 2:
            Q_q, sQ = quantize_to_int8(Q)
            K_q, sK = quantize_to_int8(K)
            V_q, sV = quantize_to_int8(V)
            suffix = "int8.bin"
        case 3:
            Q_q, sQ = quantize_to_int4(Q)
            K_q, sK = quantize_to_int4(K)
            V_q, sV = quantize_to_int4(V)
            suffix = "int4.bin"
        case _:
            print("Invalid precision level.")
            exit(1)

    ## Store inputs and weights
    store_quantized_qkv_data(f'Q_head_{suffix}', f'K_head_{suffix}', f'V_head_{suffix}', Q_q, K_q, V_q)
    store_scales(sQ, sK, sV, 'scales_head.npz')

    print("Data has been saved.")

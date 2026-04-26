import serial
import sys
import time


def read_data(port_name: str, baud_rate: int) -> list[str]:
    """
    Reads all available data from the serial port, decodes the raw bytes into strings,
    and returns the collection of read lines.

    This function attempts to establish a serial connection, reads all incoming
    data until the connection is closed or interrupted, and ensures the port
    is safely closed afterward.

    :param port_name: Name of the serial port (e.g., 'COM6' on Windows).
    :param baud_rate: The speed of the serial connection (e.g., 9600).
    :returns: A list of strings read from the serial line.
    :raises serial.serialutil.SerialException: If the port cannot be opened or configured.
    """
    ser = serial.Serial(
        port=port_name,
        baudrate=baud_rate,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        xonxoff=False,
        timeout=1,
    )
    print(f"✅ Successfully connected to {port_name}")
    data = []
    last_status_time = time.time()
    last_data_count = 0
    try:
        print("Reading data. Press CTRL+C to stop.")
        while True:
            line = ser.readline()
            if line:
                decoded_line = line.decode("utf-8").strip()
                data.append(decoded_line)

            current_time = time.time()
            if current_time - last_status_time >= 5:
                current_data_count = len(data)
                if current_data_count > last_data_count:
                    status_message = "SUCCESS: Reading new data."
                elif current_data_count == 0:
                    status_message = "WARNING: Waiting for the first data."
                else:
                    status_message = "WARNING: No new data received in last 5 seconds."
                print("\n----------------------------------------------")
                print(f"Status: {status_message}")
                print(f"Total lines read: {current_data_count}")
                print(f"New lines: {current_data_count - last_data_count}")
                print("----------------------------------------------\n")
                last_status_time = current_time
                last_data_count = current_data_count

    except KeyboardInterrupt:
        print("👋 Program interrupted by user.")
    finally:
        if "ser" in locals() and ser.is_open:
            ser.close()
            print(f"Serial port {port_name} closed.")
    return data


def save_data(data: list[str], file_name: str) -> None:
    """
    Saves the input list of strings to a new text file.

    Each element of the list is written as a separate line in the specified file.
    If the file already exists, it will be overwritten.

    :param data: The list of strings to be written to the file.
    :param file_name: The name (and path) of the file to create.
    :returns: None.
    """
    try:
        with open(file_name, "w") as f:
            for line in data:
                f.write(line + "\n")
        print(f"✅ Data successfully saved to '{file_name}'")
    except IOError as e:
        print(f"❌ An error occurred while writing to the file: {e}")


if __name__ == "__main__":
    PORT_NAME = "COM6"
    try:
        data = read_data(PORT_NAME, 9600)
        save_data(data, "data.txt")

    except serial.serialutil.SerialException as e:
        print(f"❗️ Connection Error: Could not open port {PORT_NAME}.")
        print(f"Detail: {str(e)}")
        sys.exit(1)

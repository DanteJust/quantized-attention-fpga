## Serial Data Reader and Logger
This Python script is designed to read data from a specified serial port and log the received values to a text file. It assumes the incoming data is **newline-delimited** (`\n`), treating each line as a distinct data point.

### Environment Setup
You must first create and activate a virtual environment and install the necessary package:
```bash
python3 -m venv .venv
source ./.venv/bin/activate
pip install pyserial
```

### Usage
To run the script, ensure your serial device is connected and then execute the following command:
```bash
python script.py
```

If the connection doesn't work, feel free to adjust the `PORT_NAME` and `BAUD_RATE` accordingly.

## Output
The script generates one output file upon completion:
- `data.txt` - Contains all the data received from the serial port, with each recorded value on a separate line.

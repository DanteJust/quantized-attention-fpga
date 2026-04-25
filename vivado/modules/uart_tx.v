`timescale 1ns / 1ps

module uart_tx(
    input wire clk,                             // Top level system clock
    input wire resetn,                          // Asynchronous active low reset
    output wire uart_txd,                       // UART transmit pin
    output wire uart_tx_busy,                   // Indicates when this module is busy transmitting
    input wire s_axis_tvalid,                   // Indicates whether the AXIS connection is valid
    input wire [PAYLOAD_BITS-1:0] s_axis_tdata, // Data to transmit
    output wire s_axis_tready                   // Indicates when the AXIS is ready to send data
);

// ---------------------------------------------------------------------------
// External parameters
// ---------------------------------------------------------------------------

parameter BIT_RATE = 9600;      // Bit rate of the UART line
parameter CLK_HZ = 100_000_000; // Clock frequency in hertz
parameter PAYLOAD_BITS = 8;     // Number of data bits received per UART packet
parameter STOP_BITS = 1;        // Number of stop bits indicating the end of a packet

// ---------------------------------------------------------------------------
// Internal parameters
// ---------------------------------------------------------------------------

localparam BIT_P = 1_000_000_000 * 1 / BIT_RATE;
localparam CLK_P = 1_000_000_000 * 1 / CLK_HZ;
localparam CYCLES_PER_BIT = BIT_P / CLK_P;              // Number of clock cycles per UART bit
localparam COUNT_REG_LEN = 1+$clog2(CYCLES_PER_BIT);    // Size of the registers which store sample counts and bit durations
localparam FSM_IDLE = 0;
localparam FSM_START = 1;
localparam FSM_SEND = 2;
localparam FSM_STOP = 3;

// ---------------------------------------------------------------------------
// Internal registers
// ---------------------------------------------------------------------------

reg txd_reg;                            // Internally latched value of the uart_txd line. This helps break long timing paths from the logic to the output pins.
reg [PAYLOAD_BITS-1:0] data_to_send;    // Storage for the serial data to be sent
reg [COUNT_REG_LEN-1:0] cycle_counter;  // Counter for the number of cycles over a packet bit
reg [3:0] bit_counter;                  // Counter for the number of sent bits of the packet
reg [2:0] fsm_state;                    // Current state of the internal FSM
reg [2:0] n_fsm_state;                  // Next state of the internal FSM

// ---------------------------------------------------------------------------
// AXIS related wires
// ---------------------------------------------------------------------------

assign s_axis_tready = fsm_state == FSM_IDLE;           // Indicates when module is ready to accept next byte
wire axi_handshake = s_axis_tvalid && s_axis_tready;    // Indicates whether the handshake was successfull

// ---------------------------------------------------------------------------
// FSM state selection
// ---------------------------------------------------------------------------

assign uart_tx_busy = fsm_state != FSM_IDLE;
assign uart_txd = txd_reg;
wire next_bit = cycle_counter == CYCLES_PER_BIT;
wire payload_done = bit_counter == PAYLOAD_BITS;
wire stop_done = bit_counter == STOP_BITS && fsm_state == FSM_STOP;

// Combinationally pick n_fsm_state
always @(*) begin : pick_n_fsm_state
    case(fsm_state)
        FSM_IDLE : n_fsm_state = axi_handshake ? FSM_START : FSM_IDLE;
        FSM_START: n_fsm_state = next_bit ? FSM_SEND : FSM_START;
        FSM_SEND : n_fsm_state = payload_done ? FSM_STOP : FSM_SEND ;
        FSM_STOP : n_fsm_state = stop_done ? FSM_IDLE : FSM_STOP ;
        default  : n_fsm_state = FSM_IDLE;
    endcase
end

// Sequentially pick fsm_state
always @(posedge clk) begin : pick_fsm_state
    if(!resetn) begin
        fsm_state <= FSM_IDLE;
    end else begin
        fsm_state <= n_fsm_state;
    end
end

// ---------------------------------------------------------------------------
// Updating internal register values
// ---------------------------------------------------------------------------

// Increments the bit counter each time a new bit frame is sent
always @(posedge clk) begin : pick_bit_counter
    if(!resetn) begin
        bit_counter <= 4'b0;
    end else if (( fsm_state != FSM_SEND && fsm_state != FSM_STOP ) || ( fsm_state == FSM_SEND && n_fsm_state == FSM_STOP )) begin
        bit_counter <= {COUNT_REG_LEN{1'b0}};
    end else if (next_bit && ( fsm_state == FSM_STOP || fsm_state == FSM_SEND )) begin
        bit_counter <= bit_counter + 1'b1;
    end
end

// Increments the cycle counter when sending
always @(posedge clk) begin : pick_cycle_counter
    if(!resetn || next_bit) begin
        cycle_counter <= {COUNT_REG_LEN{1'b0}};
    end else if (fsm_state != FSM_IDLE) begin
        cycle_counter <= cycle_counter + 1'b1;
    end
end

// Handles updates to the sent data register
integer i = 0;
always @(posedge clk) begin : pick_data_to_send
    if (!resetn) begin
        data_to_send <= {PAYLOAD_BITS{1'b0}};
    end else if (fsm_state == FSM_IDLE && axi_handshake) begin
        data_to_send <= s_axis_tdata;
    end else if (fsm_state == FSM_SEND && next_bit) begin
        for (i = PAYLOAD_BITS-2; i >= 0; i = i - 1) begin
            data_to_send[i] <= data_to_send[i+1];
        end
    end
end

// Updates internal value of the txd_reg
always @(posedge clk) begin : pick_txd_reg
    if (!resetn || fsm_state == FSM_IDLE || fsm_state == FSM_STOP) begin
        txd_reg <= 1'b1;
    end else if (fsm_state == FSM_START) begin
        txd_reg <= 1'b0;
    end else if (fsm_state == FSM_SEND) begin
        txd_reg <= data_to_send[0];
    end
end

endmodule

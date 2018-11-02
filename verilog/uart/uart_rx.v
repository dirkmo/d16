`timescale 1ns / 1ns

module uart_rx(
    i_clk,
    i_reset,

    o_dat,
    i_addr,
    i_we,
    i_cyc,

    rx,

    o_int
);


input i_clk;
input i_reset;
output [7:0] o_dat;
input i_addr;
input i_we;
input i_cyc;
input rx;
output reg o_int;

//-------------------------------------------------
// Baud generator
//

parameter SYS_CLK = 'd25_000_000;
parameter BAUDRATE = 'd115200;

localparam TICK = (SYS_CLK/BAUDRATE);

reg [8:0] baud_rx;
wire baud_start;

wire baud_reset = (baud_rx[8:0] == TICK[8:0]);
wire tick_rx = (baud_rx[8:0] == TICK[8:0]/2);

always @(posedge i_clk) begin
    if( baud_start || baud_reset ) begin
        baud_rx <= 0;
    end else begin
        baud_rx <= baud_rx + 1;
    end
end

//-------------------------------------------------
// Receiver

reg [7:0] rx_reg;
reg data_avail;

localparam
    IDLE = 10,
    STARTBIT = 11,
    STOPBIT = 8,
    INTERRUPT = 9,
    RECEIVE = 0;

reg [3:0] state_rx;
wire [2:0] bit_idx = state_rx[2:0];

assign baud_start = (state_rx == IDLE) && (rx == 1'b0);

reg [7:0] rx_buf; // temp receive buffer

always @(posedge i_clk) begin
    o_int <= 0;
    data_avail <= 0;
    case( state_rx )
        IDLE: // waiting for start bit
            if( rx == 1'b0 ) begin
                state_rx <= STARTBIT;
            end
        STARTBIT:
            if( tick_rx ) begin
                state_rx <= rx ? IDLE : RECEIVE;
            end
        STOPBIT:
            if( tick_rx ) begin
                state_rx <= rx ? INTERRUPT : IDLE;
            end
        INTERRUPT:
            begin
                o_int <= 1;
                data_avail <= 1;
                rx_reg <= rx_buf;
                state_rx <= IDLE;
            end
        default:
            if( tick_rx ) begin
                rx_buf[ bit_idx ] <= rx;
                state_rx <= state_rx + 1;
            end
    endcase
    if( i_reset ) begin
        state_rx <= IDLE;
    end
end

// status register
// bit0: DA data available
// bit1: OV overrun

reg [1:0] r_status;
always @(posedge i_clk)
begin
    if( data_avail ) begin
        // data received
        // if already DA flag active --> overrun
        r_status[1:0] <= { r_status[0], 1'b1 };
    end
    if( i_cyc && i_addr == 1'b0 && i_we == 1'b0 )
    begin
        // received data being read, clears overrun and data available flags
        r_status[1:0] <= 2'd0;
    end
    if( i_reset ) begin
        r_status <= 2'd0;
    end
end

//-------------------------------------------------
// bus interface

assign o_dat = i_addr ? { 6'd0, r_status } : rx_reg;


endmodule


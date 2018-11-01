`timescale 1ns / 1ns

module uart_tx(
    i_clk,
    i_reset,

    i_dat,
    o_dat,
    i_we,
    i_cyc,

    tx,

    o_int
);


input i_clk;
input i_reset;
input [7:0] i_dat;
output [7:0] o_dat;
input i_we;
input i_cyc;
output tx;
output o_int;


parameter SYS_CLK = 'd25_000_000;
parameter BAUDRATE = 'd115200;

localparam TICK = (SYS_CLK/BAUDRATE);

//---------------------------------------------
// bus slave

wire active_tx;
reg start_tx;

assign o_dat[7:0] = { 7'b0, active_tx };

reg [7:0] tx_reg; // data to send

always @(posedge i_clk)
begin
    start_tx <= 0;
    //$display("i_cyc: %d, i_we: %d, active_tx: %d, %c", i_clk, i_we, active_tx, i_dat);
    if( i_cyc && i_we && ~active_tx) begin
        tx_reg <= i_dat;
        start_tx <= 1;
    end
end


//---------------------------------------------
// uart tx

// tx baudrate generator
reg [8:0] baud_tx;

wire tick_tx = (baud_tx[8:0] == TICK[8:0]);

always @(posedge i_clk) begin
	if(start_tx || tick_tx) begin
		baud_tx <= 0;
	end else begin
		baud_tx <= baud_tx + 1;
	end
end


localparam
    SEND      = 4'd0,
    STOPBIT1  = 4'd8,
    STOPBIT2  = 4'd9,
    INTERRUPT = 4'd10,
    IDLE      = 4'd11,
    STARTBIT  = 4'd12;

reg [3:0] state_tx = IDLE;
wire [2:0] bit_idx = state_tx[2:0];

assign active_tx = (state_tx != IDLE);
assign o_int = (state_tx == INTERRUPT);

assign tx = (state_tx  < STOPBIT1) ? tx_reg[ bit_idx ] :
            (state_tx == STARTBIT) ? 1'b0 : // start bit
                                     1'b1;  // idle & stop bit

always @(posedge i_clk)
begin
    case( state_tx )
        INTERRUPT: // interrupt
            begin
                state_tx <= IDLE;
            end
        IDLE: // idle, wait for start_tx
            if( start_tx ) begin
                state_tx <= STARTBIT;
            end
        STARTBIT: // start bit
            if( tick_tx ) begin
                state_tx <= SEND;
            end
        default:
            if( tick_tx ) begin
                state_tx <= state_tx + 1;
            end
    endcase

    if( i_reset ) begin
        state_tx <= IDLE;
    end
end

endmodule


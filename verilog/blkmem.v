`timescale 1ns / 1ps

module blkmem
#(DEPTH=12)
(
    i_clk,
    i_reset,

    i_dat,
    o_dat,
    i_addr,
    i_we,
    i_cyc
);

input i_clk;
input i_reset;

input  [15:0] i_dat;
output [15:0] o_dat;
input [DEPTH-1:0] i_addr;
input i_we;
input i_cyc;


reg [15:0] mem[2**DEPTH-1:0];
	
assign o_dat = mem[i_addr];

always @(posedge i_clk)
begin
    if( i_cyc && i_we ) begin
        mem[i_addr] <= i_dat;
    end
end

endmodule

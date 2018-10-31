/* verilator lint_off PINCONNECTEMPTY */

`timescale 1ns / 1ps

module blkmem(
    i_clk,
    i_reset,

    i_dat,
    o_dat,
    i_addr,
    i_we,
    i_cyc
);

`define DEPTH 12

input i_clk;
input i_reset;

input  [15:0] i_dat;
output reg [15:0] o_dat;
input [`DEPTH-1:0] i_addr;
input i_we;
input i_cyc;

reg [15:0] mem[2**`DEPTH-1:0];

// read access with ROM
always @(i_addr)
begin
    case(i_addr)
`include "romdata.inc"
        default: o_dat = mem[i_addr];
    endcase
end


// write access
always @(posedge i_clk)
begin
    if( i_cyc && i_we ) begin
        mem[i_addr] <= i_dat;
    end
end

endmodule

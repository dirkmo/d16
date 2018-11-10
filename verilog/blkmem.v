/* verilator lint_off PINCONNECTEMPTY */

`timescale 1ns / 1ps

module blkmem(
    i_clk,

    i_dat,
    o_dat,
    i_addr,
    i_we,
    i_cyc
);

//`define ROM

`define DEPTH 16

input i_clk;

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
`ifdef ROM
`include "romdata.inc"
`endif
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

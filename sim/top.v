/* verilator lint_off PINCONNECTEMPTY */

`timescale 1ns / 1ps

module top(
    i_clk,
    i_reset,
/*
    i_dat,
    o_dat,
    i_addr,
    i_we,
    i_cyc
*/
    uart_rx,
    uart_tx
);


input i_clk;
input i_reset;
/*
input i_we;
input i_cyc
input  [15:0] i_addr;
input  [15:0] i_dat;
output [15:0] o_dat;
*/
input uart_rx;/* verilator lint_off UNUSED */
output uart_tx;

wire w_uart_tx;
assign uart_tx = w_uart_tx;

wire [15:0] o_dat;
wire [15:0] i_dat;
wire [15:0] addr;
wire cyc;
wire we;

`define NSLAVES 2
wire [`NSLAVES-1:0] slaveselect;

wire [15:0] blkmem0_dat;
wire [7:0] uart0_dat;

wire blkmem0_cyc = slaveselect[0];
wire uart0_cyc   = slaveselect[1];

assign i_dat = slaveselect[0] ? blkmem0_dat :
               slaveselect[1] ? { 8'd0, uart0_dat } :
                                16'dX;

syscon syscon0 (
    .i_addr(addr),
    .i_cyc(cyc),
    .o_slaveselect(slaveselect)
);

blkmem blkmem0 (
    .i_clk(i_clk),
    .i_dat(o_dat),
    .o_dat(blkmem0_dat),
    .i_addr(addr),
    .i_we(we),
    .i_cyc( blkmem0_cyc )
);

uart #(.SYS_CLK(1_000_000), .BAUDRATE(115200)) uart0 (
    .i_clk(i_clk),
    .i_reset(i_reset),
    .i_dat(o_dat[7:0]),
    .o_dat(uart0_dat),
    .i_addr(addr[0]),
    .i_we(we),
    .i_cyc( uart0_cyc ),
    .rx(uart_rx),
    .tx(w_uart_tx),
    .o_int()
);

d16 cpu(
    .i_clk(i_clk),
    .i_reset(i_reset),
    .i_int(),
    .o_wb_addr(addr),
    .o_wb_cyc(cyc),
    .o_wb_we(we),
    .o_wb_dat(o_dat),
    .i_wb_dat(i_dat)
);

endmodule

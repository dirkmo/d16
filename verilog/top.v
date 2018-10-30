`timescale 1ns / 1ps

module top(
    i_clk,
    i_reset,

    i_dat,
    o_dat,
    i_addr,
    i_we,
    i_cyc

    uart_rx,
    uart_tx
);

blkmem #(DEPTH(12)) blkmem0 (
    .i_clk(),
    .i_reset(),
    .i_dat(),
    .o_dat(),
    .i_addr(),
    .i_we(),
    .i_cyc()
);

uart uart0 (
    .i_clk(),
    .i_reset(),
    .i_dat(),
    .o_dat(),
    .i_addr(),
    .i_we(),
    .i_cyc(),
    .rx(),
    .tx(),
    .o_int()
);

d16 cpu(
    .i_clk(),
    .i_reset(),
    .i_int(),
    .o_wb_addr(),
    .o_wb_cyc(),
    .o_wb_we(),
    .o_wb_dat(),
    .i_wb_dat()
);

endmodule

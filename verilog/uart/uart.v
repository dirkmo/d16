/* verilator lint_off UNUSED */
`timescale 1ns / 1ns

module uart(
    i_clk,
    i_reset,
    
    i_dat,
    o_dat,
    i_addr,
    i_we,
    i_cyc,

    rx,
    tx,
    o_int
);

input  i_clk;
input  i_reset;
input  i_addr;
input  [7:0] i_dat;
output [7:0] o_dat;
input  i_we;
input  i_cyc;
input  rx;
output tx;
output [1:0] o_int;

/// uart-tx:
// i_we = 1 && i_addr = 0: write transmit register
// i_we = 0 && i_addr = 1: read status register
/// uart-rx:
// i_we = 0 && i_addr = 0: read data register
// i_we = 0 && i_addr = 1: read status register

wire uart_rx_cyc = i_cyc;
wire uart_tx_cyc = i_cyc && ( i_we ^ i_addr );
wire uart_rx_int;
wire uart_tx_int;
wire [7:0] uart_rx_dat;
wire [7:0] uart_tx_dat;
assign o_int[1:0] = { uart_tx_int, uart_rx_int };

// addr = 0: RW rx/tx reg
// addr = 1: RO status (b2: tx active, b1: overrun, b0: data avail)
assign o_dat = i_addr ? { 5'd0, uart_tx_dat[0], uart_rx_dat[1:0] }
                      : uart_rx_dat[7:0]; // addr = 0, received byte
uart_rx Uart0_rx(
    .i_clk(i_clk),
    .i_reset(i_reset),
    .o_dat(uart_rx_dat),
    .i_addr(i_addr),
    .i_we(i_we),
    .i_cyc(uart_rx_cyc),
    .rx(rx),
    .o_int(uart_rx_int)
);

uart_tx Uart0_tx(
    .i_clk(i_clk),
    .i_reset(i_reset),
    .i_dat(i_dat),
    .o_dat(uart_tx_dat),
    .i_we(i_we),
    .i_cyc(uart_tx_cyc),
    .tx(tx),
    .o_int(uart_tx_int)
);


endmodule

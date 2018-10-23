`timescale 1ns / 1ps

module top(
    clk_50mhz,
    i_reset,
    
    sram_addr,
    sram0_data,
    sram_oen,
    sram_wen,
    sram0_cen,
    sram0_ubn,
    sram0_lbn,
    
    leds
);

input clk_50mhz;
input i_reset;

output [15:0] sram_addr;
inout  [15:0] sram0_data;
output sram_oen;
output sram_wen;
output sram0_cen;
output sram0_ubn;
output sram0_lbn;

output reg [7:0] leds;

wire [15:0] addr_cpu;
wire [15:0] o_dat_cpu;
reg  [15:0] i_dat_cpu;
wire we_cpu;
wire cyc_cpu;

assign sram_addr = addr_cpu;
assign sram_oen = we_cpu;
assign sram_wen = ~we_cpu;
assign sram0_ubn = 1'b0;
assign sram0_lbn = 1'b0;
assign sram0_cen = ~cyc_cpu;

assign sram0_data = we_cpu ? o_dat_cpu : 15'hz;

//`define SLOW

`ifdef SLOW
reg [21:0] clk_div;
wire clk = clk_div[21];
always @(posedge clk_50mhz)
begin
    clk_div <= clk_div + 1;
end
`else
wire clk = clk_50mhz;
`endif

d16 cpu (
    .i_clk(clk), 
    .i_reset(i_reset), 
    .i_int(), 
    .o_wb_addr(addr_cpu), 
    .o_wb_cyc(cyc_cpu), 
    .o_wb_we(we_cpu), 
    .o_wb_dat(o_dat_cpu), 
    .i_wb_dat(i_dat_cpu)
    );


// writing
always @(posedge clk)
begin
    if(cyc_cpu && we_cpu) begin
        if( addr_cpu == 16'h7000 ) begin
            leds <= o_dat_cpu;
        end
    end
    if( i_reset ) begin
        leds <= 16'd0;
    end
end

// reading
always @(addr_cpu)
begin
    case( addr_cpu )
`include "ledtest.v"
        default: i_dat_cpu[15:0] = sram0_data[15:0];
    endcase
end


endmodule

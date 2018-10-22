`timescale 1ns / 1ps

module top(
    clk_50mhz,
    i_reset,
    o_addr,
    dat,
    
    led
);

input clk_50mhz;
input i_reset;
output [15:0] o_addr;
inout dat;

output reg [7:0] led;

wire [15:0] addr_cpu;
wire [15:0] o_dat_cpu;
reg  [15:0] i_dat_cpu;
wire we_cpu;
wire cyc_cpu;

reg [21:0] clk_div;
wire clk = clk_div[21];
always @(posedge clk_50mhz)
begin
    clk_div <= clk_div + 1;
end

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


reg [15:0] ram[0:127];

// writing
always @(posedge clk)
begin
    if(cyc_cpu && we_cpu) begin
        if( addr_cpu == 16'h7000 ) begin
            led <= o_dat_cpu;
        end else if( addr_cpu >= 16'h1000 && addr_cpu < 16'h1080 ) begin
            ram[addr_cpu[6:0]] <= o_dat_cpu[15:0];
        end
    end
    if( i_reset ) begin
        led <= 16'd0;
    end
end

// reading
always @(addr_cpu)
begin
    if( addr_cpu < 16'h400 ) begin
        case( addr_cpu )
            //i_dat_cpu[15:0] = 16'h0000;
`include "ledtest.v"
            default: i_dat_cpu[15:0] = 16'h0000;
        endcase
    end else if( addr_cpu >= 16'h1000 && addr_cpu < 16'h1080 ) begin
        i_dat_cpu[15:0] = ram[addr_cpu[6:0]];
    end
end


endmodule

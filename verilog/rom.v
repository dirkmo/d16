module rom(
    i_clk,
    o_dat,
    i_addr,
    i_cyc
);

input         i_clk;
input         i_cyc;
input   [7:0] i_addr;
output [15:0] o_dat;

always @(*)
begin
    case(i_addr)
`include "romdata.inc"
        default: o_dat = 16'd0;
    endcase
end

endmodule

module syscon(
    i_addr,
    i_cyc,
    o_slaveselect
);

parameter NSLAVES = 2;

input [15:0] i_addr;
input i_cyc;
output reg [NSLAVES-1:0] o_slaveselect;

always @(*)
begin
    o_slaveselect = 0;
    if( i_cyc ) begin
        if( i_addr < 'h1000 )       o_slaveselect = 1;
        else if( i_addr == 'h1000 ) o_slaveselect = 2;
    end
end

endmodule

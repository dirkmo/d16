/* verilator lint_off UNUSED */
/* verilator lint_off UNDRIVEN */
/* verilator lint_off PINCONNECTEMPTY */

module d16(
    i_clk,
    i_reset,
    i_int,

    o_wb_addr,
    o_wb_cyc,
    o_wb_we,
    o_wb_dat,
    i_wb_dat,
    i_wb_ack,
    i_wb_err
);

input i_clk;
input i_reset;
input i_int;

// wishbone wires
input i_wb_ack;
input i_wb_err;
input  [15:0] i_wb_dat;
output [15:0] o_wb_addr;
output [15:0] o_wb_dat;
output o_wb_we;
output o_wb_cyc;

// stacks
reg [6:0] ds = 0; // Bit 6 ist Überlaufbit
reg [15:0] D[63:0];
wire [5:0] ds_idx = ds[5:0];
wire [5:0] ds_TOSidx = ds_idx - 1; // D TOS index
wire [5:0] ds_NOSidx = ds_idx - 2; // D NOS index

reg [6:0] rs = 0; // Bit 6 ist Überlaufbit
reg [15:0] R[63:0];
wire [5:0] rs_idx = rs[5:0];
wire [5:0] rs_TOSidx = rs_idx - 1; // R TOS index

reg [15:0] pc;

reg [15:0] ir;

reg [2:0] flags; // flags[0] immer 1 setzen! Für einfacheres check conditions Testen

reg fetch;
reg execute;

// instruction bit decoding
wire       itype  = ir[15];
wire [14:0]  imm  = ir[14:0];

wire  [1:0] cond  = ir[14:13];
wire  [1:0] dsp   = ir[12:11];
wire        rsp   = ir[10];
wire  [2:0] src   = ir[9:7];
wire  [2:0] dst   = ir[6:4];
wire  [3:0] aluop = ir[3:0];

wire [15:0] alu = 16'd0;

// bus source selection
wire [15:0] bus =
        src == 3'b000 ? R[rs_TOSidx] :
        src == 3'b001 ? D[ds_TOSidx] :
        src == 3'b010 ? pc :
        src == 3'b011 ? { 9'd0, ds }:
        src == 3'b100 ? i_wb_dat :
        src == 3'b101 ? alu :
        src == 3'b110 ? 16'd0
                      : 16'd0; // src == 3'b111

assign o_wb_dat = bus;

// dst block
always @(posedge i_clk)
begin
    if( execute ) begin
        if( itype == 1'b1 ) begin
            // regular instruction
            if( flags[ cond ] == 1'b1 ) begin
                case( dst )
                    3'b000: ; // nothing
                    3'b001: begin
                        // R[rs] <= bus, rs++
                        R[rs_idx] <= bus;
                        rs <= rs + 1'b1;
                    end
                    3'b010: D[ds_idx] <= bus;
                    3'b011: ; // will be handled in D stack pointer block
                    3'b100: pc <= bus;
                    3'b101: o_wb_we <= 1'b1;
                    3'b110: rs <= { 1'b0, bus[5:0] };
                    3'b111: begin // SWAP
                        D[ds_TOSidx] <= D[ds_NOSidx];
                        D[ds_NOSidx] <= D[ds_TOSidx];
                    end
                endcase
            end
        end else begin
            // immediate instruction
            D[ds_idx] <= { 1'b0, imm };
        end
    end
end

// D stack pointer ds
always @(posedge i_clk)
begin
    if( execute ) begin
        if( itype == 1'b1 ) begin
            if( flags[ cond ] == 1'b1 ) begin
                case(dsp)
                    2'b00: ;
                    2'b01: ds <= ds + 1;
                    2'b10: ds <= ds - 1;
                    2'b11: ds <= ds - 2;
                endcase
                if( dst == 3'b011 ) begin
                    ds <= { 1'b0, bus[5:0] };
                end
            end
        end else begin
            // immediate instruction
            ds <= ds + 1;
        end
    end 
end

endmodule


/*
[15] | [14-0]
 0   | val#

[15] | [14-13] | [12-11] | [10] | [9-7] | [6-4] | [3-0]
 1   |  cond   |   dsp   |  rsp |  src  |  dst  |  alu
 */

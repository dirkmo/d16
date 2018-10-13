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

`define ZERO  1
`define NEG   2
`define CARRY 3

reg [3:0] flags;

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

// wishbone bus
reg wb_we = 0;
reg wb_cyc = 0;
assign o_wb_dat  = bus;
assign o_wb_we   = execute ? wb_we   : 1'b0;
assign o_wb_cyc  = execute ? wb_cyc  : 1'b1;
assign o_wb_addr = execute ? D[ds_NOSidx] : pc;

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

// instruction fetch
always @(posedge i_clk)
begin
    execute <= ~execute;
    if( execute == 1'b0 ) begin
        ir <= i_wb_dat;
    end
    if( i_reset ) begin
        execute <= 1'b0;
    end
end

// dst block
always @(posedge i_clk)
begin
    wb_we <= 1'b0;
    wb_cyc <= 1'b0;
    if( execute ) begin
        pc <= pc + 1;
        if( itype == 1'b1 ) begin
            // regular instruction
            if( flags[ cond ] == 1'b1 ) begin
                case( dst )
                    3'b000: ; // nothing
                    3'b001: begin
                        // push(R, bus)
                        R[rs_idx] <= bus;
                        rs <= rs + 1'b1;
                    end
                    3'b010: D[ds_idx] <= bus;
                    3'b011: ; // will be handled in D stack pointer block
                    3'b100: pc <= bus;
                    3'b101: begin
                        // mem <= bus
                        wb_we <= 1'b1;
                        wb_cyc <= 1'b1;
                    end
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
    if( i_reset ) begin
        pc <= 16'd0;
        rs <= 7'd0;
        ds <= 7'd0;
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

// alu
reg alu_carry;
wire alu_zero = alu == 16'd0;
wire alu_neg  = alu[15];
wire [15:0] T = D[ds_TOSidx];
wire [15:0] N = D[ds_NOSidx];
always @(*)
begin
    alu_carry = flags[`CARRY];
    case( aluop )
        4'd0: // ADD
            { alu_carry, alu } = {1'b0, T } + { 1'b0, N };
        4'd1: // ADC
            { alu_carry, alu } = {1'b0, T } + { 1'b0, N } + { 16'd0, flags[`CARRY] };
        4'd2: // AND
            alu = T & N;
        4'd3: // OR
            alu = T | N;
        4'd4: // XOR
            alu = T ^ N;
        4'd5: // INV
            alu = ~T;
        4'd6: // LSL
            alu = N << T;
        4'd7: // LSR
            alu = N >> T;
        default: alu = 0;
    endcase
end

always @(posedge i_clk)
begin
    if( execute ) begin
        flags <= { alu_carry, alu_neg, alu_zero, 1'b1 };
    end
    if( i_reset ) begin
        flags <= 4'b0001;
    end
end

endmodule


/*
[15] | [14-0]
 0   | val#

[15] | [14-13] | [12-11] | [10] | [9-7] | [6-4] | [3-0]
 1   |  cond   |   dsp   |  rsp |  src  |  dst  |  alu
 */

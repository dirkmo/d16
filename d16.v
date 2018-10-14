/* verilator lint_off UNUSED */
/* averilator lint_off UNDRIVEN */
/* averilator lint_off PINCONNECTEMPTY */

module d16(
    i_clk,
    i_reset,
    i_int,

    o_wb_addr,
    o_wb_cyc,
    o_wb_we,
    o_wb_dat,
    i_wb_dat
);

input i_clk;
input i_reset;
input i_int;

// wishbone wires
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
wire [15:0] T = D[ds_TOSidx];
wire [15:0] N = D[ds_NOSidx];

reg [6:0] rs = 0; // Bit 6 ist Überlaufbit
reg [15:0] R[63:0];
wire [5:0] rs_idx = rs[5:0];
wire [5:0] rs_TOSidx = rs_idx - 1; // R TOS index

reg [15:0] pc;
wire [15:0] pc1 = pc + 1;

reg [15:0] ir;

`define CPUSTATE_RESET 2'b00
`define CPUSTATE_FETCH 2'b01
`define CPUSTATE_EXECUTE 2'b10

reg [1:0] cpu_state;


// instruction bit decoding
wire       itype  = ir[15];
wire [14:0]  imm  = ir[14:0];

wire  [1:0] dsp   = ir[14:13];
wire        rsp   = ir[12];
wire  [2:0] src   = ir[11:9];
//wire       unused = ir[8];
wire  [3:0] dst   = ir[7:4];
wire  [3:0] aluop = ir[3:0];

reg [15:0] alu;

// wishbone bus
reg wb_we = 0;
reg wb_cyc = 0;
assign o_wb_dat  = bus;
assign o_wb_we   = cpu_state == `CPUSTATE_EXECUTE ? wb_we  : 1'b0;
assign o_wb_cyc  = cpu_state == `CPUSTATE_EXECUTE ? wb_cyc :
                   cpu_state == `CPUSTATE_FETCH   ? 1'b1   : 1'b0;
assign o_wb_addr = cpu_state == `CPUSTATE_EXECUTE ? D[ds_TOSidx] : pc;

// bus source selection
wire [15:0] bus =
        src == 3'd0 ? R[rs_TOSidx] :
        src == 3'd1 ? T :
        src == 3'd2 ? pc :
        src == 3'd3 ? { 9'd0, ds }:
        src == 3'd4 ? i_wb_dat :
        src == 3'd5 ? alu :
        src == 3'd6 ? T == 16'd0 ? N : pc1 // JMPZ
                    : T[15] ? N : pc1; // JMPL

// instruction fetch
always @(posedge i_clk)
begin
    if( cpu_state == `CPUSTATE_FETCH ) begin
        ir <= i_wb_dat;
    end
end

// state machine
always @(posedge i_clk)
begin
    case ( cpu_state )
        `CPUSTATE_RESET:   cpu_state <= `CPUSTATE_FETCH;
        `CPUSTATE_FETCH:   cpu_state <= `CPUSTATE_EXECUTE;
        `CPUSTATE_EXECUTE: cpu_state <= `CPUSTATE_FETCH;
        default: cpu_state <= `CPUSTATE_RESET;
    endcase
    if ( i_reset ) begin
        cpu_state <= `CPUSTATE_RESET;
    end
end

// dst block
always @(posedge i_clk)
begin
    wb_we <= 1'b0;
    wb_cyc <= 1'b0;
    if( cpu_state == `CPUSTATE_EXECUTE ) begin
        pc <= pc + 1;
        if( itype == 1'b1 ) begin
            // regular instruction
            if ( rsp ) begin
                rs <= rs - 1'b1;
            end
            case( dst )
                4'd0: begin
                    // push(R, bus)
                    R[rs_idx] <= bus;
                    rs <= rs + 1'b1;
                end
                4'd1: D[ds_idx] <= bus;
                4'd2: D[ds_TOSidx] <= bus;
                4'd3: D[ds_NOSidx] <= bus;
                4'd4: ; // will be handled in D stack pointer block
                4'd5: pc <= bus;
                4'd6: begin
                    // mem <= bus
                    wb_we <= 1'b1;
                    wb_cyc <= 1'b1;
                end
                4'd7: rs <= { 1'b0, bus[5:0] };
                4'd8: begin
                    D[ds_TOSidx] <= {15'd0, alu_carry };
                    D[ds_NOSidx] <= bus;
                end
                default: ;
            endcase
        end else begin
            // immediate instruction
            D[ds_idx] <= { 1'b0, imm };
        end
    end
    if( cpu_state == `CPUSTATE_RESET ) begin
        pc <= 16'd0;
        rs <= 7'd0;
        ds <= 7'd0;
    end
end

// D stack pointer ds
always @(posedge i_clk)
begin
    if( cpu_state == `CPUSTATE_EXECUTE ) begin
        if( itype == 1'b1 ) begin
            case(dsp)
                2'b00: ;
                2'b01: ds <= ds + 1;
                2'b10: ds <= ds - 1;
                2'b11: ds <= ds - 2;
            endcase
            if( dst == 4'd4 ) begin
                ds <= { 1'b0, bus[5:0] };
            end
        end else begin
            // immediate instruction
            ds <= ds + 1;
        end
    end 
end

// alu
reg alu_carry;
always @(*)
begin
    case( aluop )
        4'd0: // ADD
            alu = T + N;
        4'd1: // ADC
            { alu_carry, alu } = {1'b0, T } + { 1'b0, N };
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
        4'd8: // SUB
            alu = N - T;
        4'd9: // SBC
            { alu_carry, alu } = { N[15], N } - { T[15], T };
        default: alu = 0;
    endcase
end

endmodule


/*
[15] | [14-0]
 0   | val#

[15] | [14-13] | [12] | [11-9] |   8   | [7-4] | [3-0]
 1   |   dsp   |  rsp |  src   |unused | dst   |  alu
 */

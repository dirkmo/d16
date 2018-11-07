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
reg [6:0] ds = 0; // Bit 6 ist berlaufbit
reg [15:0] D[63:0];
wire [5:0] ds_idx = ds[5:0];
wire [5:0] ds_TOSidx = ds_idx - 1; // D TOS index
wire [5:0] ds_NOSidx = ds_idx - 2; // D NOS index
wire [15:0] T = D[ds_TOSidx];
wire [15:0] N = D[ds_NOSidx];

reg [6:0] rs = 0; // Bit 6 ist berlaufbit
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
wire       itype     = ir[15];
wire [14:0]  imm     = ir[14:0];

wire  [1:0] dsp      = ir[14:13];
wire        rsp      = ir[12];
wire  [3:0] src      = ir[11:8];
wire  [3:0] dst      = ir[7:4];
wire  [3:0] aluop    = ir[3:0];
wire  [3:0] pick_idx = ds_TOSidx - ir[3:0];

reg [15:0] alu;
reg alu_carry;

wire mem_read_access = itype && (src == 4'd4);
wire mem_write_access = itype && (dst == 4'd6);
wire mem_access = mem_read_access || mem_write_access;
wire [15:0] bus;

// wishbone bus
assign o_wb_dat  = bus;
assign o_wb_we   = cpu_state == `CPUSTATE_EXECUTE ? mem_write_access  : 1'b0;

assign o_wb_cyc  = cpu_state == `CPUSTATE_EXECUTE ? mem_access :
                   cpu_state == `CPUSTATE_FETCH   ? 1'b1   : 1'b0;
assign o_wb_addr = cpu_state == `CPUSTATE_EXECUTE ? D[ds_TOSidx] : pc;

// bus source selection
assign bus[15:0] =
        src == 4'd0  ? R[rs_TOSidx] :
        src == 4'd1  ? T :
        src == 4'd2  ? pc1 :
        src == 4'd3  ? { 9'd0, ds }:
        src == 4'd4  ? i_wb_dat :
        src == 4'd5  ? alu :
        src == 4'd6  ? N == 16'd0 ? T : pc1 : // JMPZ
        src == 4'd7  ? N[15] ? T : pc1 : // JMPL
        src == 4'd8  ? N :
        src == 4'd9  ? N == 16'd0 ? pc1 : T : // JMPNZ
        src == 4'd10 ? D[pick_idx] : // PICK
                      16'd0;

// cond: used in dst block for conditional branches.
// only push address on RS when cond == 1
wire cond = (src == 4'd6) ? N == 16'd0 :
            (src == 4'd7) ? N[15] : 1'b1;

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
    //wb_we <= 1'b0;
    //wb_cyc <= 1'b0;
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
                    // kommt zu spt!! geht hier nicht.
                    //wb_we <= 1'b1;
                    //wb_cyc <= 1'b1;
                end
                4'd7: rs <= { 1'b0, bus[5:0] };
                4'd8: begin
                    D[ds_TOSidx] <= {15'd0, alu_carry };
                    D[ds_NOSidx] <= bus;
                end
                4'd9: if ( cond ) begin
                    // only push address on RS when condition true
                    R[rs_idx] <= pc1;
                    rs <= rs + 1'b1;
                    pc <= bus;
                end
                4'd10: begin
                    D[ds_TOSidx] <= D[ds_NOSidx];
                    D[ds_NOSidx] <= D[ds_TOSidx];
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
                // stack pointer pushen bringt nichts, stack ist nicht memory
                // mapped
                ds <= { 1'b0, bus[5:0] };
            end
        end else begin
            // immediate instruction
            ds <= ds + 1;
        end
    end 
    if( i_reset ) begin
        ds <= 7'd0;
    end
end

// alu
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

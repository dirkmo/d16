#include "Vd16.h"
#include "verilated.h"
#include "testbench.h"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

class d16test : public TESTBENCH<Vd16> {
public:

    void updateBusState(Wishbone16 *bus) {
        bus->addr = m_core->o_wb_addr;
        bus->we = m_core->o_wb_we;
        bus->cyc = m_core->o_wb_cyc;
        m_core->i_wb_err = bus->err;
        m_core->i_wb_ack = bus->ack;
        if( bus->cyc ) {
            if(bus->we) {
                bus->dat = m_core->o_wb_dat;
            } else {
                m_core->i_wb_dat = bus->dat;
            }
        }
    }

    void print_ds() {
        printf("D: <%d> ", m_core->d16__DOT__ds);
        for ( int i=0; i<m_core->d16__DOT__ds; i++) {
            printf("%02X ", m_core->d16__DOT__D[i]);
        }
        printf("\n");
    }

    void print_rs() {
        printf("R: <%d> ", m_core->d16__DOT__rs);
        for ( int i=0; i<m_core->d16__DOT__rs; i++) {
            printf("%02X ", m_core->d16__DOT__R[i]);
        }
        printf("\n");
    }
};

enum CONSTANTS {
    OP          = 1 << 15,
    // conditions
    COND_NONE   = 0 << 13,
    COND_ZERO   = 1 << 13,
    COND_NEG    = 2 << 13,
    COND_CARRY  = 3 << 13,
    // data stack pointer ops
    DSP_NONE    = 0 << 11,
    DSP_INC     = 1 << 11,
    DSP_DEC     = 2 << 11,
    DSP_DEC2    = 3 << 11,
    // return stack pointer ops
    RSP_NONE    = 0 << 10,
    RSP_DEC     = 1 << 10,
    // src: bus driver
    SRC_RTOS    = 0 << 7,
    SRC_DTOS    = 1 << 7,
    SRC_PC      = 2 << 7,
    SRC_DSP     = 3 << 7,
    SRC_MEM     = 4 << 7,
    SRC_ALU     = 5 << 7,
    // dst: destination register
    DST_RS      = 0 << 4, // push return stack element
    DST_DS      = 1 << 4, // push data stack element
    DST_DS1     = 2 << 4, // set TOS (for eg. LOAD)
    DST_DS2     = 3 << 4, // set NOS (for eg. ADD)
    DST_DSP     = 4 << 4, // set data stack pointer
    DST_PC      = 5 << 4, // set PC
    DST_MEM     = 6 << 4, // write memory
    DST_RSP     = 7 << 4, // set return stack pointer
    // alu
    ALU_ADD     = 0 << 0,
    ALU_ADC     = 1 << 0,
    ALU_AND     = 2 << 0,
    ALU_OR      = 3 << 0,
    ALU_XOR     = 4 << 0,
    ALU_INV     = 5 << 0,
    ALU_LSL     = 6 << 0,
    ALU_LSR     = 7 << 0,
};

/*
[15] | [14-0]
 0   | val#

[15] | [14-13] | [12-11] | [10] | [9-7] | [6-4] | [3-0]
 1   |  cond   |   dsp   |  rsp |  src  |  dst  |  alu
 */

enum OPCODES {
    DUP   = OP | COND_NONE  | DSP_INC | SRC_DTOS | DST_DS,
    DUPZ  = OP | COND_ZERO  | DSP_INC | SRC_DTOS | DST_DS,
    DUPN  = OP | COND_NEG   | DSP_INC | SRC_DTOS | DST_DS,
    DUPC  = OP | COND_CARRY | DSP_INC | SRC_DTOS | DST_DS,

    ADD   = OP | COND_NONE  | DSP_DEC | SRC_ALU  | DST_DS2 | ALU_ADC,
};

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    d16test *tb = new d16test();
    tb->opentrace("trace.vcd");
    
    Wishbone16 *bus = new Wishbone16;
    uint16_t big_endian_data[] = {
        0x1, DUP, ADD
    };

    Memory16 mem(1024);
    mem.write(0, big_endian_data, sizeof(big_endian_data));
    
    tickcounter = &tb->m_tickcount;

    tb->reset();
    tb->tick();

    for( int i = 0; i<6; i++) {
        tb->updateBusState(bus);
        bus->ack = true;
        printf("%s\n", tb->m_core->d16__DOT__cpu_state == 0 ? "RESET" :
                       tb->m_core->d16__DOT__cpu_state == 1 ? "FETCH" :
                       tb->m_core->d16__DOT__cpu_state == 2 ? "EXECUTE" :
                       "UNKNOWN");
        mem.task( (bus->addr < 1024) && bus->cyc, bus);
        tb->updateBusState(bus);
        tb->tick();
        tb->print_ds();
        //print_rs();
    }

    delete bus;
    
    return 0;
}

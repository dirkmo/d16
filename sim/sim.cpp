#include "Vd16.h"
#include "verilated.h"
#include "testbench.h"
#include "../include/d16.h"
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

using namespace std;

class sim : public TESTBENCH<Vd16> {
public:

    void updateBusState(Wishbone16& bus) {
        bus.addr = m_core->o_wb_addr;
        bus.we = m_core->o_wb_we;
        bus.cyc = m_core->o_wb_cyc;
        if( bus.cyc ) {
            if(bus.we) {
                bus.dat = m_core->o_wb_dat;
            } else {
                m_core->i_wb_dat = bus.dat;
            }
        }
    }

    void print_ds() {
        printf("D: <%d> ", m_core->d16__DOT__ds);
        for ( int i=0; i<m_core->d16__DOT__ds; i++) {
            printf("%04X ", m_core->d16__DOT__D[i]);
        }
        printf("\n");
    }

    void print_rs() {
        printf("R: <%d> ", m_core->d16__DOT__rs);
        for ( int i=0; i < m_core->d16__DOT__rs; i++) {
            printf("%04X ", m_core->d16__DOT__R[i]);
        }
        printf("\n");
    }

};


int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    Memory16 mem(0x7000);
    Uart uart(0x7000);
    sim *tb = new sim();
    Wishbone16 bus;

    vector<uint16_t> prog = {
        0x0018, 0x0004, 0xC190, 0xFFFF, 0xA110, 0x8420, 0xA110, 0x0017, 0xE650,
        0xA110, 0x7000, 0xE860, 0x0008, 0xC537, 0xA110, 0x0017, 0xE650, 0x7000,
        0xE860, 0x0001, 0xC530, 0x0004, 0xC150, 0x9050, 0x6148, 0x6C6C, 0x206F,
        0x6557, 0x746C, 0x0021, 0x0000,
    };

    mem.clear();
    mem.init(prog);
    tb->reset();

    tb->opentrace("trace.vcd");

    tb->tick();
    int icount = 0;

    while(icount++ < 350) {

        uint16_t pc = tb->m_core->d16__DOT__pc;

        if( mem.read(pc) == 0xFFFF ) {
            printf("Simulation done.\n");
            break;
        }

        if (tb->m_core->d16__DOT__cpu_state == 1) {
            printf("pc: %04X\n", pc);
        }
        printf("%s\n", tb->m_core->d16__DOT__cpu_state == 0 ? "RESET" :
                tb->m_core->d16__DOT__cpu_state == 1 ? "FETCH" :
                tb->m_core->d16__DOT__cpu_state == 2 ? "EXECUTE" :
                "UNKNOWN");

        tb->updateBusState(bus);
        mem.task( bus.cyc, bus);
        uart.task( bus.cyc, bus);
        tb->updateBusState(bus);
        tb->tick();

        if (tb->m_core->d16__DOT__cpu_state == 1) {
            tb->print_ds();
            tb->print_rs();
        }
    }

    return 0;
}

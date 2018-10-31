#include "Vtop.h"
#include "verilated.h"
#include "testbench.h"
#include "../include/d16.h"
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

using namespace std;

class sim : public TESTBENCH<Vtop> {
public:

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

    top *tb = new top();


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

        tb->tick();

    }

    return 0;
}

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
};


int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    sim *tb = new sim();


    tb->reset();

    tb->opentrace("trace.vcd");

    tb->tick();
    int icount = 0;

    while(icount++ < 350) {

        uint16_t pc = tb->m_core->top__DOT__cpu__DOT__pc;

        if( tb->m_core->top__DOT__cpu__DOT__ir == 0xFFFF ) {
            printf("Simulation done.\n");
            break;
        }

        if (tb->m_core->top__DOT__cpu__DOT__cpu_state == 1) {
            printf("pc: %04X\n", pc);
        }

        tb->tick();

    }

    return 0;
}

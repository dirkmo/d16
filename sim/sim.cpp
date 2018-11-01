#include "Vtop.h"
#include "verilated.h"
#include "testbench.h"
#include "../include/d16.h"
#include "../include/protothreads.h"
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

using namespace std;

class sim : public TESTBENCH<Vtop> {
public:
};

class Uart {
public:
    Uart( sim& _computer ) : computer(_computer) {
    }

    sim computer;
};

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    sim *tb = new sim();


    tb->reset();

    tb->opentrace("trace.vcd");

    tb->tick();
    int icount = 0;

    while(icount++ < 3500) {

        uint16_t pc = tb->m_core->v__DOT__cpu__DOT__pc;

        if( tb->m_core->v__DOT__cpu__DOT__ir == 0xFFFF ) {
            printf("Simulation done.\n");
            break;
        }

        if (tb->m_core->v__DOT__cpu__DOT__cpu_state == 1) {
            printf("(%d) pc: %04X\n", icount, pc);
        }

        tb->tick();

    }

    return 0;
}

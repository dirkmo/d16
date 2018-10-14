#include "Vd16.h"
#include "verilated.h"
#include "testbench.h"
#include "d16.h"
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

using namespace std;

class d16test : public TESTBENCH<Vd16> {
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

struct CpuInternals {
    uint8_t flags[3];
    vector<uint16_t> D;
    vector<uint16_t> R;
};

class TestData {
    public:
    TestData( CpuInternals _cpu , vector<uint16_t> _prog) {
        cpu = _cpu;
        prog = _prog;
    }
    CpuInternals cpu;
    vector<uint16_t> prog;
};

class Test {
    public:

    Test( d16test *ptb ) : mem(0xFFFF) {
        m_ptb = ptb;
    }

    void addTest( TestData td ) {
        m_vTests.push_back( td );
    }

    bool isSimulationDone() {
        uint16_t pc = m_ptb->m_core->d16__DOT__pc;
        return mem.read(pc) == 0xFFFF;
    }

    void doCycle() {
        m_ptb->updateBusState(bus);
        mem.task( (bus.addr < 1024) && bus.cyc, bus);
        m_ptb->updateBusState(bus);
        m_ptb->tick();
    }

    bool doTest(int idx) {
        printf("Executing test %i\n", idx);
        TestData& td = m_vTests[idx];
        mem.clear();
        mem.init(td.prog);
        m_ptb->reset();
        m_ptb->tick();
        int icount = 0;
        while(icount++ < 10) {

            if( isSimulationDone() ) {
                printf("Simulation finished\n");
                return false;
            }

            printf("%s\n", m_ptb->m_core->d16__DOT__cpu_state == 0 ? "RESET" :
                    m_ptb->m_core->d16__DOT__cpu_state == 1 ? "FETCH" :
                    m_ptb->m_core->d16__DOT__cpu_state == 2 ? "EXECUTE" :
                    "UNKNOWN");

            doCycle();

            m_ptb->print_ds();

        }
        return false;
    }

    int testCount() { return m_vTests.size(); }

    vector<TestData> m_vTests;
    Memory16 mem;
    Wishbone16 bus;
    d16test *m_ptb;
};

void setupTests(Test& tester) {
    tester.addTest(
        (TestData) {
            .cpu =  { .flags = { 0, 0, 0 }, .D = { 1, 1 }, .R = { } },
            .prog = { 1, DUP, HALT }
        }
    );
}


int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    d16test *tb = new d16test();
    tb->opentrace("trace.vcd");

    Test Tester(tb);
    setupTests(Tester);
    Tester.doTest(0);

    return 0;
}

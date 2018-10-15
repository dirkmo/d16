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

struct CpuInternals {
    uint16_t pc;
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

bool v16_compare( vector<uint16_t> a, vector<uint16_t> b) {
    vector<uint16_t>::iterator ai = a.begin();
    vector<uint16_t>::iterator bi = b.begin();
    while( ai != a.end() && bi != b.end() ) {
        if( *ai != *bi ) {
            return false;
        }
        ai++;
        bi++;
    }
    return ai == a.end() && bi == b.end();
}

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
        return (mem.read(pc) == 0xFFFF);
    }

    void doCycle() {
        m_ptb->updateBusState(bus);
        mem.task( bus.cyc, bus);
        m_ptb->updateBusState(bus);
        m_ptb->tick();
    }

    bool testResults(int idx) {
        vector<uint16_t> D, R;
        uint8_t ds = m_ptb->m_core->d16__DOT__ds;
        uint8_t rs = m_ptb->m_core->d16__DOT__rs;
        if(ds < 0) {
            printf("DS Stack underflow\n");
            return false;
        }
        if(rs < 0) {
            printf("RS Stack underflow\n");
            return false;
        }
        for(int i=0; i<ds; i++) {
            D.push_back(m_ptb->m_core->d16__DOT__D[i]);
        }
        for(int i=0; i<rs; i++) {
            R.push_back(m_ptb->m_core->d16__DOT__R[i]);
        }

        bool dres = v16_compare(D, m_vTests[idx].cpu.D);
        if( !dres ) {
            printf("DS stack error\n");
            return false;
        }

        bool rres = v16_compare(R, m_vTests[idx].cpu.R);
        if( !rres ) {
            printf("RS stack error\n");
            return false;
        }
        if ( m_vTests[idx].cpu.pc != m_ptb->m_core->d16__DOT__pc ) {
            printf("pc error!\n");
            return false;
        }
        return true;
    }

    bool doTest(int idx) {
        printf("\n======Executing test %i======\n", idx);
        TestData& td = m_vTests[idx];
        mem.clear();
        mem.init(td.prog);
        m_ptb->reset();
        m_ptb->tick();
        int icount = 0;
        while(icount++ < 20) {

            if( isSimulationDone() ) {
                break;
            }

            if (m_ptb->m_core->d16__DOT__cpu_state == 1) {
                printf("pc: %04X\n", m_ptb->m_core->d16__DOT__pc);
            }
            printf("%s\n", m_ptb->m_core->d16__DOT__cpu_state == 0 ? "RESET" :
                    m_ptb->m_core->d16__DOT__cpu_state == 1 ? "FETCH" :
                    m_ptb->m_core->d16__DOT__cpu_state == 2 ? "EXECUTE" :
                    "UNKNOWN");
            doCycle();

            m_ptb->print_ds();
            m_ptb->print_rs();
        }
        printf("Simulation finished\n");
        return testResults(idx);
    }

    int testCount() { return m_vTests.size(); }

    vector<TestData> m_vTests;
    Memory16 mem;
    Wishbone16 bus;
    d16test *m_ptb;
};

void setupTests(Test& tester) {
    // 0 SUB JMPZ
    tester.addTest( (TestData) {
            .cpu =  { .pc = 5, .D = {  }, .R = { } },
            .prog = { 6, 100, 99, SUB, JMPZ, HALT, 0x1234, HALT }
        }
    );
    // 1 SUB JMPZ
    tester.addTest( (TestData) {
            .cpu =  { .pc = 7, .D = { 0x1234 }, .R = { } },
            .prog = { 6, 100, 100, SUB, JMPZ, HALT, 0x1234, HALT }
        }
    );
    // 2 SUB JMPZ
    tester.addTest( (TestData) {
            .cpu =  { .pc = 5, .D = { }, .R = { } },
            .prog = { 6, 99, 100, SUB, JMPZ, HALT, 0x1234, HALT }
        }
    );
    // 3 SUB JMPL
    tester.addTest( (TestData) {
            .cpu =  { .pc = 5, .D = { }, .R = { } },
            .prog = { 6, 100, 100, SUB, JMPL, HALT, 0x1234, HALT }
        }
    );
    // 4 SUB JMPL
    tester.addTest( (TestData) {
            .cpu =  { .pc = 7, .D = { 0x1234 }, .R = { } },
            .prog = { 6, 100, 101, SUB, JMPL, HALT, 0x1234, HALT }
        }
    );
    // 5 SUB JMPL
    tester.addTest( (TestData) {
            .cpu =  { .pc = 5, .D = { }, .R = { } },
            .prog = { 6, 101, 100, SUB, JMPL, HALT, 0x1234, HALT }
        }
    );
    // 6 CALL
    tester.addTest( (TestData) {
            .cpu =  { .pc = 5, .D = { 1 }, .R = { 2 } },
            .prog = { 4, CALL, HALT, HALT, 0x1, HALT }
        }
    );
    // 7 CALL RET
    tester.addTest( (TestData) {
            .cpu =  { .pc = 3, .D = { 1, 2 }, .R = { } },
            .prog = { 4, CALL, 0x2, HALT, 0x1, RET, HALT }
        }
    );
    // 8 DUP
    tester.addTest( (TestData) {
            .cpu =  { .pc = 2, .D = { 4, 4 }, .R = { } },
            .prog = { 4, DUP, HALT }
        }
    );
    // 9 SWAP
    tester.addTest( (TestData) {
            .cpu =  { .pc = 3, .D = { 1, 4 }, .R = { } },
            .prog = { 4, 1, SWAP, HALT }
        }
    );
    // 10 DROP
    tester.addTest( (TestData) {
            .cpu =  { .pc = 3, .D = { 4 }, .R = { } },
            .prog = { 4, 1, DROP, HALT }
        }
    );
    // 11 PUSHRS
    tester.addTest( (TestData) {
            .cpu =  { .pc = 2, .D = { }, .R = { 4 } },
            .prog = { 4, PUSHRS, HALT }
        }
    );
    // 12 DROPRS
    tester.addTest( (TestData) {
            .cpu =  { .pc = 5, .D = { }, .R = { 2 } },
            .prog = { 1, 2, PUSHRS, PUSHRS, DROPRS, HALT }
        }
    );
    // 13 POPRS
    tester.addTest( (TestData) {
            .cpu =  { .pc = 6, .D = { 1, 2 }, .R = {  } },
            .prog = { 1, 2, PUSHRS, PUSHRS, POPRS, POPRS, HALT }
        }
    );
    // 14 LOAD
    tester.addTest( (TestData) {
            .cpu =  { .pc = 3, .D = { 0x1234, 0x1234 }, .R = {  } },
            .prog = { 0x1234, 0, LOAD, HALT }
        }
    );
    // 15 STORE
    tester.addTest( (TestData) {
            .cpu =  { .pc = 6, .D = { 1, 2 }, .R = {  } },
            .prog = { 0x1, 2, 0, STORE, 0, LOAD, HALT }
        }
    );
}


int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    d16test *tb = new d16test();
    tb->opentrace("trace.vcd");

    Test Tester(tb);
    setupTests(Tester);
    for( int i=0; i<Tester.testCount(); i++) {
        if( Tester.doTest(i) == false ) {
            printf("ERROR\n");
            return 1;
        }
    }
    printf("\nsuccess.\n");
    return 0;
}

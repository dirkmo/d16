#include <verilated.h>
#include <verilated_vcd_c.h>
#include <iostream>
#include "Vuart_rx.h"
#include "protothreads.h"

using namespace std;

Vuart_rx *top;
VerilatedVcdC *trace;

vluint64_t main_time = 0;

double sc_time_stamp () {       // Called by $time in Verilog
    return main_time;           // converts to double, to match
                                // what SystemC does
}

void opentrace(const char *vcdname, Vuart_rx *core) {
    Verilated::traceEverOn(true);
    trace = new VerilatedVcdC;
    core->trace(trace, 99);
    trace->open(vcdname);
}

#define T(t) (main_time < t/2)

static bool posedge() {
    static uint8_t last = 0;
    if( last != top->i_clk ) {
        last = top->i_clk;
        return top->i_clk;
    }
    return false;
}

static bool negedge() {
    static uint8_t last = 0;
    if( last != top->i_clk ) {
        last = top->i_clk;
        return !top->i_clk;
    }
    return false;
}

static struct pt pt;
static PT_THREAD(actions_pt(struct pt *pt)) {
    PT_BEGIN(pt);
    top->i_reset = 1;
    PT_WAIT_WHILE(pt, T(20));
    PT_WAIT_UNTIL(pt, posedge());
    top->i_reset = 0;
    PT_WAIT_UNTIL(pt, posedge());
    top->i_cyc = 1;
    /**/
    PT_WAIT_UNTIL(pt, posedge());
    top->i_cyc = top->i_we = 0;
    PT_WAIT_UNTIL(pt, posedge());
    PT_END(pt);
}

int main(int argc, char** argv) {
    cout << "Simulating uart_rx..." << endl;
    Verilated::commandArgs(argc, argv);

    top = new Vuart_rx;
    opentrace("trace.vcd", top);

    PT_INIT(&pt, NULL);
    while (!Verilated::gotFinish()) {
        if ((main_time % 10) == 0) {
            top->i_clk = 1;
        }
        if ((main_time % 10) == 5) {
            top->i_clk = 0;
        }

        if( PT_SCHEDULE(actions_pt(&pt)) == 0 ) break;

        top->eval();

        trace->dump(static_cast<vluint64_t>(main_time*2));
        trace->flush();

        //cout << top->out << endl;
        main_time++;
    }

    top->final();
    
    delete top;
}

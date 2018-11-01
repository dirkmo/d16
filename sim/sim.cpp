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

class Uart {
public:
    enum CONSTANTS {
        SYS_CLK   = 1000000,
        BAUDRATE  = 115200,
        UART_TICK = (SYS_CLK / BAUDRATE),
    };

    Uart( uint8_t *_tx, uint8_t *_rx, uint8_t *_clk) : tx(*_tx), rx(*_rx), clk(*_clk) {
        PT_INIT(&tx_pt, NULL);
        PT_INIT(&rx_pt, NULL);
    }

    void task() {
        send();
        receive();
    }

    PT_THREAD(send()) {
        PT_BEGIN(&tx_pt);
        tx = 1;
        PT_END(&tx_pt);
    }

    PT_THREAD(receive()) {
        PT_BEGIN(&rx_pt);
        while(1) {
            PT_WAIT_UNTIL(&rx_pt, clk);
            // clk = 1
            PT_WAIT_WHILE(&rx_pt, clk);
            // clk = 0
        }
        PT_END(&rx_pt);
    }

    uint8_t& tx;
    uint8_t& rx;

    uint8_t& clk;

    struct pt tx_pt;
    struct pt rx_pt;
};

class sim : public TESTBENCH<Vtop> {
public:
    sim() : uart(&m_core->uart_rx, &m_core->uart_tx, &m_core->i_clk) {

    }

	virtual void tick() override {
		m_tickcount++;

		m_core->i_clk = 0;
		m_core->eval();

        uart.task();
		
		if(m_trace) m_trace->dump(static_cast<vluint64_t>(10*m_tickcount-2));

		m_core->i_clk = 1;
		m_core->eval();
        uart.task();
		if(m_trace) m_trace->dump(static_cast<vluint64_t>(10*m_tickcount));

		m_core->i_clk = 0;
		m_core->eval();
        uart.task();
		if (m_trace) {
			m_trace->dump(static_cast<vluint64_t>(10*m_tickcount+5));
			m_trace->flush();
		}
	}
    Uart uart;
};

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    sim *tb = new sim();


    tb->reset();

    tb->opentrace("trace.vcd");

    tb->tick();
    int icount = 0;

    while(icount++ < 35) {

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

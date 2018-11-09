#ifndef __SIM_H
#define __SIM_H

#include <sstream>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cassert>

#include "Vtop.h"
#include "verilated.h"
#include "testbench.h"
#include "../include/d16.h"
#include "uart.h"


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

    uint16_t getPC() { return m_core->v__DOT__cpu__DOT__pc; }

    string getDS() {
        stringstream ss;
        for( int i = 0; i < m_core->v__DOT__cpu__DOT__ds; i++ ) {
            uint8_t d = m_core->v__DOT__cpu__DOT__D[i];
            ss << std::hex << (int)d << " ";
        }
        ss << std::dec << "(" << (int)m_core->v__DOT__cpu__DOT__ds << ")";
        return ss.str();
    }

    Uart uart;
};

#endif

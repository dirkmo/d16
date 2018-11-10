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
    struct {
        bool trace = false;
        bool debug = false;
        bool run = false;
        bool step = false;
        bool verbose = false;
    } options;

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

    uint16_t getPC() { return m_core->top__DOT__cpu__DOT__pc; }

    uint16_t getMem(uint16_t addr) {
        return m_core->top__DOT__blkmem0__DOT__mem[addr];
    }

    void setMem(uint16_t addr, uint16_t dat ) {
        m_core->top__DOT__blkmem0__DOT__mem[addr] = dat;
    }

    uint16_t getIR() { return m_core->top__DOT__cpu__DOT__ir; }

    vector<uint16_t> getDS() {
        vector<uint16_t> v;
        for( int i = 0; i < m_core->top__DOT__cpu__DOT__ds; i++ ) {
            v.push_back(m_core->top__DOT__cpu__DOT__D[i]);
        }
        return v;
    }

    vector<uint16_t> getRS() {
        vector<uint16_t> v;
        for( int i = 0; i < m_core->top__DOT__cpu__DOT__rs; i++ ) {
            v.push_back(m_core->top__DOT__cpu__DOT__R[i]);
        }
        return v;
    }

    Uart uart;
};

#endif

#include <stdint.h>
#include <verilated_vcd_c.h>
#include <stdio.h>
#include <vector>

using namespace std;

uint64_t* tickcounter = NULL;

uint64_t tickcount() {
    if( tickcounter ) {
        return *tickcounter*10;
    }
	return 0;
}

template<class MODULE>	class TESTBENCH {
public:
	uint64_t m_tickcount;
	VerilatedVcdC *m_trace;
	MODULE	*m_core;

	TESTBENCH(void) {
		Verilated::traceEverOn(true);
		m_core = new MODULE;
		m_trace = NULL;
		m_tickcount = 0l;
		tickcounter = &m_tickcount;
	}

	virtual ~TESTBENCH() {
		delete m_core;
		m_core = NULL;
	}

		// Open/create a trace file
	virtual	void opentrace(const char *vcdname) {
		if (!m_trace) {
			m_trace = new VerilatedVcdC;
			m_core->trace(m_trace, 99);
			m_trace->open(vcdname);
		}
	}

	// Close a trace file
	virtual void close(void) {
		if (m_trace) {
			m_trace->close();
			m_trace = NULL;
		}
	}

	virtual void reset() {
		m_core->i_reset = 1;
		// Make sure any inheritance gets applied
		this->tick();
		m_core->i_reset = 0;
	}

	virtual void tick() {
		// Increment our own internal time reference
		m_tickcount++;

		// Make sure any combinatorial logic depending upon
		// inputs that may have changed before we called tick()
		// has settled before the rising edge of the clock.
		m_core->i_clk = 0;
		m_core->eval();
		
		if(m_trace) m_trace->dump(static_cast<vluint64_t>(10*m_tickcount-2));

		// Toggle the clock

		// Rising edge
		m_core->i_clk = 1;
		m_core->eval();
		if(m_trace) m_trace->dump(static_cast<vluint64_t>(10*m_tickcount));

		// Falling edge
		m_core->i_clk = 0;
		m_core->eval();
		if (m_trace) {
			// This portion, though, is a touch different.
			// After dumping our values as they exist on the
			// negative clock edge ...
			m_trace->dump(static_cast<vluint64_t>(10*m_tickcount+5));
			//
			// We'll also need to make sure we flush any I/O to
			// the trace file, so that we can use the assert()
			// function between now and the next tick if we want to.
			m_trace->flush();
		}
	}

	virtual bool done() {
        return Verilated::gotFinish();
    }
};

class Wishbone16 {
public:
    uint32_t addr;
    bool cyc;
    bool we;
    uint16_t dat;
};

class Uart {
public:
	Uart( uint16_t _base ) : base(_base) {}

	void task( bool sel, Wishbone16 &bus ) {
		if( sel ) {
            if( bus.cyc && bus.addr == base ) {
                if( bus.we ) {
                    printf("---------------> UART write %02X (%c)\n", bus.dat & 0xFF, bus.dat & 0xFF);
                } else {
	                //printf("%lu: mem read %04X: %04X\n", tickcount(), addr, bus.dat);
					printf("UART read\n");
                }
            }
        }
	}
	uint16_t base;
};

class Memory16 {
public:
    uint16_t *mem = NULL;
    uint16_t size;

    Memory16(uint16_t _size) : size(_size) {
        mem = new uint16_t[size];
    }

    ~Memory16() {
        delete[] mem;
    }

	void clear() {
		memset(mem, 0, size);
	}

	void init(vector<uint16_t>& dat) {
		int i = 0;
		for( auto d: dat) {
			mem[i++] = d;
		}
	}

    void write(uint16_t addr, uint16_t *dat, uint16_t len) {
        while(len--) {
            mem[addr++] = *dat++;
			assert(addr < size);
        }
    }

	uint16_t read(uint16_t addr) {
		assert(addr < size);
		return mem[addr];
	}

    void task(bool sel, Wishbone16 &bus) {
        if( sel ) {
            if( bus.cyc && bus.addr < size) {
                uint32_t addr = bus.addr;
                if( bus.we ) {
                    mem[addr] = bus.dat;
                    printf("%lu: mem write %04X: %04X\n", tickcount(), addr, bus.dat);
                } else {
                    bus.dat = 0;
                    bus.dat = mem[addr];
	                printf("%lu: mem read %04X: %04X\n", tickcount(), addr, bus.dat);
                }
            }
        }
    }
};

#include "Vdcpu.h"
#include "verilated.h"
#include "testbench.h"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

class d16test : public TESTBENCH<Vdcpu> {
public:

    void updateBusState(Wishbone *bus) {
        bus->addr = m_core->o_wb_addr;
        bus->we = m_core->o_wb_we;
        bus->cyc = m_core->o_wb_cyc;
        m_core->i_wb_err = bus->err;
        m_core->i_wb_ack = bus->ack;
        if( bus->cyc ) {
            if(bus->we) {
                bus->dat = m_core->o_wb_dat;
            } else {
                m_core->i_wb_dat = bus->dat;
            }
        }
    }
};


int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    dcputest *tb = new d16test();
    tb->opentrace("trace.vcd");
    
    Wishbone *bus = new Wishbone;
    uint16_t big_endian_data[] = {
        0
    };

    Memory mem(1024);
    mem.set(0, big_endian_data, sizeof(big_endian_data));
    
    tickcounter = &tb->m_tickcount;

    tb->reset();
    tb->tick();

    for( int i = 0; i<60; i++) {
        tb->updateBusState(bus);
        bus->ack = false;
        mem.task( (bus->addr < 1024) && bus->cyc, bus);
        tb->updateBusState(bus);
        tb->tick();
    }

    delete bus;
    
    return 0;
}

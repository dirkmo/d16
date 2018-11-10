#include "sim.h"
#include "debug.h"
#include <fstream>

using namespace std;

void loadMemory(sim *tb, uint16_t addr, string fn) {
    ifstream ifs(fn, ios::binary);
    uint16_t data[0x10000];
    ifs.read((char*)data, sizeof(data));
    for( uint16_t i = 0; i < ifs.gcount()/2; i++ ) {
        tb->setMem(addr++, data[i]);
    }
}

void parseCommandLine(sim *tb, int argc, char **argv) {
    // -d debug mode
    // -t enable trace
    string fn = "boot.bin";
    for( int i = 1; i < argc; i++ ) {
        string s = argv[i];
        if ( s == "-t" ) {
            tb->options.trace = true;
        } else if( s == "-d" ) {
            tb->options.debug = true;
        } else if( s == "-i" ) {
            i++;
            if( i >= argc ) {
                cerr << "Missing filename." << endl;
                exit(1);
            }
            fn = argv[i];
        }
    }
    loadMemory(tb, 0, fn);
}

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    sim *tb = new sim();
    
    parseCommandLine(tb, argc, argv);
    
    tb->reset();
    
    if( tb->options.trace ) {
        tb->opentrace("trace.vcd");
    }

    tb->tick();

    int icount = 0;

    tb->uart.sendbyte('D');
    tb->uart.sendbyte('1');
    tb->uart.sendbyte('6');
    tb->uart.sendbyte('!');

    while(icount++ < 150) {
        if( tb->options.debug && !tb->options.run) {
            debugPrompt(tb);
        }

        if( !tb->options.debug || tb->options.run || tb->options.step ) {
            tb->options.step = false;

            if( tb->m_core->top__DOT__cpu__DOT__ir == 0xFFFF ) {
                printf("Simulation done.\n");
                break;
            }
            
            tb->tick();
            tb->tick();

            if( tb->onBreakpoint() ) {
                tb->options.run = false;
                tb->options.step = false;
                cout << "Halted on breakpoint." << endl;
            }
        }
    }
    delete tb;
    return 0;
}

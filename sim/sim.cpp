#include "sim.h"
#include "debug.h"

using namespace std;

struct {
    bool trace = false;
    bool debug = false;
} options;


void parseCommandLine(int argc, char **argv) {
    // -d debug mode
    // -t enable trace
    for( int i = 1; i < argc; i++ ) {
        string s = argv[i];
        if ( s == "-t" ) {
            options.trace = true;
        } else if( s == "-d" ) {
            options.debug = true;
        }
    }
}


int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    parseCommandLine(argc, argv);
    
    sim *tb = new sim();
    tb->reset();
    
    if( options.trace ) {
        tb->opentrace("trace.vcd");
    }

    tb->tick();
    int icount = 0;

    tb->uart.sendbyte('D');
    tb->uart.sendbyte('1');
    tb->uart.sendbyte('6');
    tb->uart.sendbyte('!');

    while(icount++ < 150) {
        if( options.debug ) {
            debugPrompt(tb);
        }
        if( tb->m_core->top__DOT__cpu__DOT__ir == 0xFFFF ) {
            printf("Simulation done.\n");
            break;
        }

        if (tb->m_core->top__DOT__cpu__DOT__cpu_state == 1) {
            cout << std::dec << tb->getTickCounter() << ": 0x" << std::hex << tb->getPC();
            cout << " -- DS: " << tb->getDS() << endl;
        }

        tb->tick();
    }
    delete tb;
    return 0;
}

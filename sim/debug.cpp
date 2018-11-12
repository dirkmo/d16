#include <iostream>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <iomanip>
#include "sim.h"
#include "opcodes.h"

using namespace std;

static void printCommands() {
    cout << "Commands:" << endl;
    cout << "  d [addr] [len]  dump memory" << endl;
    cout << "  D [addr] [len]  disassemble memory" << endl;
    cout << "  b <addr>        enable breakpoint at address" << endl;
    cout << "  r               run" << endl;
    cout << "  s               step" << endl;
    cout << "  ds              print data stack" << endl;
    cout << "  rs              print return stack " << endl;
    cout << "  verbose         verbose mode" << endl;
}

static string trim(const string& _s) {
    string s = _s;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}

static void tokenize( const string& _s, vector<string>& vs ) {
    string s;
    for( auto c: _s ) {
        if( isspace(c) ) {
            if( s.length() > 0 ) {
                vs.push_back(s);
                s = "";
            }
        } else {
            s += c;
        }
    }
    if( s.length() > 0 ) {
        vs.push_back(s);
    }
}

static void dumpMemory( sim *tb, vector<string>& vs ) {
    uint16_t i;

    int len = 4;
    int addr = tb->getPC();
    if( vs.size() > 1 ) {
        try {
            addr = stoi(vs[1], NULL, 0);
            if( vs.size() > 2 ) {
                len = stoi(vs[2], NULL, 0);
            }
        } catch(...) {
            cout << "Invalid arguments" << endl;
            return;
        }
    }
    for( i = 0; i < len; i++ ) {
        if( i % 8 == 0 ) {
            if( i > 0 ) {
                // for( int j=0; j < 8; j++ ) {
                //     auto val = tb->getMem(addr + i + j);
                //     char c = val >> 8;
                //     if( isgraph(c) ) cout << c; else cout << ".";
                //     c = val;
                //     if( isgraph(c) ) cout << c; else cout << ".";
                // }
                cout << endl;
            }
            cout << hex << setfill('0') << setw(4) << addr+i << ": ";
        }
        cout << hex << setfill('0') << setw(4) << tb->getMem(addr + i) << " ";
    }
    cout << endl;
}

static void disassemble( sim *tb, vector<string>& vs ) {
    uint16_t i;

    int len = 16;
    int addr = tb->getPC();
    if( vs.size() > 1 ) {
        try {
            addr = stoi(vs[1], NULL, 0);
            if( vs.size() > 2 ) {
                len = stoi(vs[2], NULL, 0);
            }
        } catch(...) {
            cout << "Invalid arguments" << endl;
            return;
        }
    }
    for( i = 0; i < len; i++ ) {
        if( i % 16 == 0 ) {
            cout << endl << hex << setfill('0') << setw(4) << addr+i << ": ";
        }
        cout << mnemonic(tb->getMem(addr + i)) << " ";
    }
    cout << endl;
}

void print_stack(sim* tb, const vector<uint16_t>& v) {
    cout << dec << "<" << v.size() << ">";
    for( auto d: v ) {
        cout << hex << " " << d;
    }
    cout << endl;
}

void toggleBreakpoint(sim* tb, vector<string>& vs) {
    uint16_t addr = tb->getPC();
    if( vs.size() > 1 ) {
        try {
            addr = stoi(vs[1], NULL, 0);
        } catch(...) {
            cout << "Invalid breakpoint address" << endl;
            return;
        }
    }
    if( tb->toggleBreakpoint(addr) ) {
        cout << "Set breakpoint on " << hex << addr << endl;
    } else {
        cout << "Removed breakpoint on " << hex << addr << endl;
    }
}

void debugPrompt( sim *tb ) {
    string s;
    if( tb->options.verbose) {
        print_stack(tb, tb->getDS());
    }
    cout << hex << setfill('0') << setw(4) << tb->getPC()
        << " [" << mnemonic(tb->getMem(tb->getPC())) << "] > ";
    getline(cin, s);
    s = trim(s);
    vector<string> vs;
    tokenize( s, vs );
    if( vs.size() == 0 ) {
        return;
    }
    if( vs[0] == "help" ) {
        printCommands();
    } else if( vs[0] == "d" && vs.size() > 1 ) {
        dumpMemory(tb, vs);
    } else if( vs[0] == "D" ) {
        disassemble(tb, vs);
    } else if( vs[0] == "b" ) {
        // breakpoint
        toggleBreakpoint(tb, vs);
    } else if( vs[0] == "r" ) {
        tb->options.run = true;
    } else if( vs[0] == "s" ) {
        // step
        tb->options.step = true;
    } else if( vs[0] == "ds" ) {
        print_stack(tb, tb->getDS());
    } else if( vs[0] == "rs" ) {
        print_stack(tb, tb->getRS());
    } else if( vs[0] == "verbose" ) {
        tb->options.verbose = !tb->options.verbose;
        cout << "verbose " << (tb->options.verbose ? "on" : "off") << endl;
    } else {
        cout << "Error" << endl;
    }
}


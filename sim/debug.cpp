#include <iostream>
#include <algorithm> 
#include <cctype>
#include <locale>
#include "sim.h"

using namespace std;

static void printCommands() {
    cout << "Commands:" << endl;
    cout << "  d <addr> [len]  dump memory" << endl;
    cout << "  b <addr>        enable breakpoint at address" << endl;
    cout << "  r               run" << endl;
    cout << "  s               step" << endl;
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

static void dumpMemory( sim *tb, uint16_t addr, uint16_t len ) {
    uint16_t i;
    for( i = 0; i < len; i++ ) {
        if( i % 16 == 0 ) {
            cout << endl << "0x" << hex << addr+i << ": ";
        }
        cout << tb->m_core->top__DOT__blkmem0__DOT__mem[i] << " ";
    }
}

void debugPrompt( sim *tb ) {
    string s;
    cout << "d16> ";
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
        int len = 16;
        int addr;
        try {
            addr = stoi(vs[1]);
            if( vs.size() > 2 ) {
                len = stoi(vs[2]);
            }
        } catch(...) {
            cout << "Invalid arguments" << endl;
            return;
        }
        dumpMemory( tb, addr, len );
    } else if( vs[0] == "b" ) {
    } else if( vs[0] == "r" ) {
    } else if( vs[0] == "s" ) {
    } else {
        cout << "Error" << endl;
    }
}


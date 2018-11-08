#include <iostream>
#include "sim.h"

using namespace std;


void debugPrompt( sim *tb ) {
    string s;
    cout << "d16> ";
    cin >> s;
    if( s == "help" ) {
        printCommands();
    }
}

void printCommands() {
    cout << "Commands:" << endl;
    cout << "  d <addr> [len]  dump memory" << endl;
    cout << "  b <addr>        enable breakpoint at address" << endl;
    cout << "  r               run" << endl;
    cout << "  s               step" << endl;
}

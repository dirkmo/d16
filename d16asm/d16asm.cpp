#include <iostream>
#include "d16asm.h"

list<CmdBase*> cmdlist;

void addIdentifier(string name) {
    CmdIdentifier *ident = new CmdIdentifier(name);
    cmdlist.push_back(ident);
}

void addNumber(uint16_t val) {
    auto num = new CmdNumber(val);
    cmdlist.push_back(num);
}

void addLabel(string name) {
    auto label = new CmdLabel(name);
    cmdlist.push_back(label);
}

void addKeyword(CmdKeyword::Keyword keyword) {
    auto key = new CmdKeyword(keyword);
    cmdlist.push_back(key);
}

void addOrg( uint16_t addr ) {
    auto org = new CmdOrg(addr);
    cmdlist.push_back(org);
}

void addDs( uint16_t size ) {
    auto ds = new CmdDs(size);
    cmdlist.push_back(ds);
}

void addEqu( string name, uint16_t val ) {
    auto equ = new CmdEqu( name, val );
    cmdlist.push_back(equ);
}

void addDw() {
    auto dw = new CmdDw();
    cmdlist.push_back(dw);
}

int main( int argc, char **argv ) {
    printf("d16 cpu assembler\n");
    yyparse();

    for( auto cmd: cmdlist ) {
        cout << cmd->getString() << endl;
    }

    return 0;
}

#include <iostream>
#include "d16asm.h"
#include "assemble.h"

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
    dw->addPayload(listPayload);
    cmdlist.push_back(dw);
}

int main( int argc, char **argv ) {
    printf("d16 cpu assembler\n");
    string outname = "d16output.s";

    if( argc > 1 ) {
        outname = argv[1];
        extern FILE *yyin;
        if( !(yyin = fopen(argv[1], "r")) ) {
            perror(argv[1]);
            return 1;
        }
    }
    yyparse();

    assemble(cmdlist, outname);

    return 0;
}

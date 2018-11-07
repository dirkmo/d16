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

void addKeyword(d16::OPCODES opcode) {
    auto key = new CmdKeyword(opcode);
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

void addPick(uint8_t idx) {
    if(idx > 0xF ) {
        printf("Invalid PICK index %u\n",idx);
        assert(idx < 0x10);
    }
    auto opcode = static_cast<d16::OPCODES>(static_cast<int>(d16::PICK) + idx);
    auto pick = new CmdKeyword(opcode);
    cmdlist.push_back(pick);
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

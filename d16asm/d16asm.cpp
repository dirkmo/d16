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

int main( int argc, char **argv ) {
    printf("d16 cpu assembler\n");
    yyparse();

    for( auto cmd: cmdlist ) {
        cout << cmd->getString() << endl;
    }

    return 0;
}

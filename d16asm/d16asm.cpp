#include "d16asm.h"

mapT dictIdent;
vector<Cell> cells;

void addIdentifier(string name, uint16_t val) {
    Identifier& id = dictIdent[name];
    if( id.hasValue ) {
        printf("ERROR: Identifier %s already defined\n", name.c_str());
    } else {
        Identifier newId(val);
        dictIdent[name] = newId;
    }
}

void addIdentifier(string name) {
    Identifier& id = dictIdent[name];
}

void addNumber(uint16_t val, uint16_t pc) {
    if( cells[pc].hasValue ) {
        printf("ERROR: Address %d has already a value\n", pc);
    } else {
        cells[pc] = CellNumber(val);
    }
}

int main( int argc, char **argv ) {
    Cell emptyCell;
    cells.resize( 0x10000, emptyCell );
    yyparse();
    return 0;
}

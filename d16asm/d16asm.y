%{
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include "d16asm.h"

extern int yylex();

using namespace std;

struct Identifier {
    Identifier() : hasValue(false) {
        printf("New Identifier\n");
    }
    Identifier(uint16_t _val) : hasValue(true), val(_val) {
        printf("New Identifier with val %d\n", _val);
    }
    uint16_t val;
    bool hasValue;
};

class Cell {
public:
    enum Type { None, Ident, Number, Literal, Label, Keyword };

    Cell() : type(None) {}

    uint16_t value = 0;
    bool hasValue = false;
    Type type;
};

class CellNumber : public Cell {
public:
    CellNumber(uint16_t val) {
        type = Cell::Number;
        value = val;
        hasValue = true;
    }
};


uint16_t pc = 0;

typedef map<string, Identifier> mapT;

mapT dictIdent;

vector<Cell> cells;

void addIdentifier(string name, uint16_t val);
void addIdentifier(string name);
void addNumber(uint16_t val, uint16_t pc);

%}

%union {
	long l;
	char *s;
}

%token <l> NUMBER
%token <s> IDENTIFIER LITERAL LABEL
%token EOL ORG EQU DW DROP JMP CALL RET


%start S

%%

S:
 | S line
 ;


line: EOL
    | keyword
    | NUMBER { addNumber($1, pc++); }
    | LABEL  { addIdentifier($1, pc); }
    | IDENTIFIER { printf("Identifier: %s\n", $1); }
    | directive
    ;


keyword: DROP { printf("Keyword DROP\n"); }
       | JMP { printf("Keyword JMP\n"); }
       | CALL { printf("Keyword CALL\n"); }
       | RET { printf("Keyword RET\n"); }
       ;


directive: ORG NUMBER EOL { printf("directive is ORG %ld\n", $2); }
         | equ
         | dw
         ;


equ: EQU IDENTIFIER NUMBER EOL { printf("directive is EQU %s %ld\n", $2, $3); }
   | EQU IDENTIFIER LITERAL EOL { printf("directive is EQU %s %s\n", $2, $3); }
   ;


dw: DW dwdata { printf("directive is DW\n"); }
  ;


dwdata: NUMBER { printf("%ld ", $1); }
      | LITERAL { printf("%s ", $1); }
      | IDENTIFIER { printf("%s ", $1); }
      | dwdata ',' NUMBER { printf("%ld, ", $3); }
      | dwdata ',' LITERAL { printf("%s, ", $3); }
      | dwdata ',' IDENTIFIER { printf("%s ", $3); }
      ;

%%

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

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

int main( int argc, char **argv ) {
    Cell emptyCell;
    cells.resize( 0x10000, emptyCell );
    yyparse();
    return 0;
}


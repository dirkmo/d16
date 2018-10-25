#ifndef __D16ASM_H
#define __D16ASM_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <string>

using namespace std;


/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);
extern int yylex();
extern int yyparse();



void addIdentifier(string name, uint16_t val);
void addIdentifier(string name);
void addNumber(uint16_t val, uint16_t pc);

struct Identifier {
    Identifier() : hasValue(false) {
        printf("New Identifier\n");
    }
    Identifier(uint16_t _val) : hasValue(true), val(_val) {
        printf("New Identifier with val %d\n", _val);
    }
    bool hasValue;
    uint16_t val;
};

typedef map<string, Identifier> mapT;

class Cell {
public:
    enum Type { None, Ident, Number, Literal, Label, Keyword };

    uint16_t value;
    bool hasValue;
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


#endif


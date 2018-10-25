#ifndef __D16ASM_H
#define __D16ASM_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include <list>

using namespace std;


/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);
extern int yylex();
extern int yyparse();

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

class CmdBase {
public:
    enum Type { None, Number, Directive, Keyword, Ident, Label };
    
    virtual string getString() = 0;
    
    Type type;
};

class CmdNumber : public CmdBase {
public:
    CmdNumber( uint16_t _val ) : value(_val) {
        type = CmdBase::Number;
    }

    virtual string getString() override {
        return to_string(value);
    }

    uint16_t value;
};

class CmdIdentifier : public CmdBase {
public:
    CmdIdentifier( string _name ) : name(_name) {
        type = CmdBase::Ident;
    }
    
    virtual string getString() override {
        return name;
    }

    string name;
};

class CmdKeyword : public CmdBase {
public:
    enum Keyword { DROP, JMP, CALL, RET };

    CmdKeyword( Keyword key ) : keyword(key) {
        type = CmdBase::Keyword;
    }

    string getMnemonic() {
        const static string mnemonics[] = {
            "DROP", "JMP", "CALL", "RET",
        };
        return mnemonics[keyword];
    }

    virtual string getString() override {
        return getMnemonic();
    }

    Keyword keyword;
};

class CmdLabel : public CmdBase {
public:
    CmdLabel( string label ) {
        type = CmdBase::Label;
        name = label;
        if( name.back() == ':' ) {
            name.pop_back();
        }
    }

    virtual string getString() override {
        return name;
    }

    string name;
};

void addIdentifier(string name);
void addNumber(uint16_t val);
void addLabel(string name);
void addKeyword(CmdKeyword::Keyword keyword);

#endif

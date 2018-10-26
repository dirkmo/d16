#ifndef __D16ASM_H
#define __D16ASM_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <assert.h>

using namespace std;

struct dwPayload {
    enum Type { Ident, Number };

    dwPayload( string _id ) : type(Ident), sIdent(_id), value(0) {}
    dwPayload( uint16_t num ) : type(Number), sIdent(""), value(num) {}
    
    string toString() {
        if( type == Ident ) {
            return sIdent;
        }
        return to_string(value);
    }

    Type type;
    string sIdent;
    uint16_t value;
};


/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);
extern int yylex();
extern int yyparse();

extern list<dwPayload> listPayload;

class CmdBase {
public:
    enum Type { None, Number, Equ, Ds, Dw, Org, Keyword, Ident, Label };
    
    virtual string getString() = 0;

    virtual uint16_t getValue() {
        return value;
    }
    
    Type type;

    uint16_t addr;

    uint16_t value;
    bool hasValue = false;
};

class CmdNumber : public CmdBase {
public:
    CmdNumber( uint16_t _val ) {
        type = CmdBase::Number;
        value = _val;
        hasValue = true;
    }

    virtual string getString() override {
        return to_string(value);
    }
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

    bool isExtended() {
        return value > 0x7FFF;
    }

    string name;
};

class CmdEqu : public CmdBase {
public:
    CmdEqu( string _name, uint16_t val ) {
        type = CmdBase::Equ;
        value = val;
        name = _name;
    }
    bool isExtended() {
        return value > 0x7FFF;
    }
    virtual string getString() override {
        return ".EQU " + name + " " + to_string(value);
    }
    string name;
};

class CmdOrg : public CmdBase {
public:
    CmdOrg( uint16_t _addr ) {
        addr = _addr;
    }

    virtual string getString() override {
        char buf[10];
        sprintf(buf, ".ORG %u", addr);
        return string(buf);
    }
};

class CmdDs : public CmdBase {
public:
    CmdDs( uint16_t _size ) {
        size = _size;
    }

    virtual string getString() override {
        char buf[10];
        sprintf(buf, ".DS %u", size);
        return string(buf);
    }
    uint16_t size;
};

class CmdIdentifier : public CmdBase {
public:
    CmdIdentifier( string _name ) : name(_name), ref(NULL) {
        type = CmdBase::Ident;
    }
    
    virtual string getString() override {
        return name;
    }

    void setReference(CmdBase *_ref) {
        assert( ref == NULL );
        ref = _ref;
    }

    bool isExtended() {
        assert( ref );
        if( ref->type == CmdBase::Equ ) {
            CmdEqu* equ = static_cast<CmdEqu*>(ref);
            return equ->isExtended();
        } else if( ref->type == CmdBase::Label ) {
            CmdLabel* label = static_cast<CmdLabel*>(ref);
            return label->isExtended();
        }
    }

    string name;
    CmdBase *ref;
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

class CmdDw : public CmdBase {
public:
    CmdDw() {
        type = CmdBase::Dw;
    }
    virtual string getString() override {
        string s = ".DW";
        for( auto it: payload ) {
            s += " " + it.toString();
        }
        return s;
    }
    void addPayload( const list<dwPayload>& list ) {
        payload = list;
    }
    list<dwPayload> payload;
};

void addIdentifier(string name);
void addNumber(uint16_t val);
void addLabel(string name);
void addKeyword(CmdKeyword::Keyword keyword);
void addOrg( uint16_t addr );
void addDs( uint16_t size );
void addEqu( string name, uint16_t val );
void addDw();


#endif


#ifndef __D16ASM_H
#define __D16ASM_H

#include <cstdio>
#include <cstdarg>
#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <sstream>
#include <iomanip>
#include <assert.h>
#include "opcodes.h"

using namespace std;

struct dwPayload {
    enum Type { Ident, Number, Literal };

    dwPayload( string str, Type _type ) : value(0) {
        assert( _type == Ident || _type == Literal );
        type = _type;
        if( type == Literal ) {
            // remove quotes
            literal = str.substr(1, str.length() - 2 );
        } else {
            ident = str;
        }
    }
    dwPayload( uint16_t num ) : type(Number), literal(""), value(num) {}
    
    string toString() {
        if( type == Literal ) {
            return literal;
        }
        if( type == Ident ) {
            return ident;
        }
        char buf[8];
        sprintf(buf, "0x%04X", value);
        return string(buf);
    }

    Type type;
    string literal;
    string ident;
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
    string name;
    uint32_t lineno;
};

class CmdNumber : public CmdBase {
public:
    CmdNumber( uint16_t _val ) {
        type = CmdBase::Number;
        value = _val;
        hasValue = true;
        lineno = yylineno;
    }

    virtual string getString() override {
        char buf[8];
        sprintf(buf, "0x%04X", value);
        return string(buf);
    }
};

class CmdReference : public CmdBase {
public:
    virtual bool isExtended() = 0;
};

class CmdLabel : public CmdReference {
public:
    CmdLabel( string label ) {
        lineno = yylineno;
        type = CmdBase::Label;
        name = label;
        if( name.back() == ':' ) {
            name.pop_back();
        }
    }
    virtual bool isExtended() override {
        return addr > 0x7FFF;
    }
    virtual string getString() override {
        return name + ":";
    }
    virtual uint16_t getValue() override {
        return addr;
    }

};

class CmdEqu : public CmdReference {
public:
    CmdEqu( string _name, uint16_t val ) {
        lineno = yylineno;
        type = CmdBase::Equ;
        value = val;
        addr = val;
        name = _name;
    }
    virtual bool isExtended() override {
        return value > 0x7FFF;
    }
    virtual string getString() override {
        char buf[16];
        sprintf(buf, ".EQU %s 0x%04X", name.c_str(), value);
        return string(buf);
    }
};

class CmdOrg : public CmdBase {
public:
    CmdOrg( uint16_t _addr ) {
        lineno = yylineno;
        addr = _addr;
        type = CmdBase::Org;
    }

    virtual string getString() override {
        char buf[10];
        sprintf(buf, ".ORG %04X", addr);
        return string(buf);
    }
};

class CmdDs : public CmdBase {
public:
    CmdDs( uint16_t _size ) {
        lineno = yylineno;
        size = _size;
        type = CmdBase::Ds;
    }

    virtual string getString() override {
        char buf[16];
        sprintf(buf, ".DS 0x%04X", size);
        return string(buf);
    }
    uint16_t size;
};

class CmdIdentifier : public CmdBase {
public:
    CmdIdentifier( string _name, int16_t _offset ) : ref(NULL), offset(_offset) {
        lineno = yylineno;
        type = CmdBase::Ident;
        name = _name;
    }
    
    virtual string getString() override {
        stringstream s;
        s << name;
        if( offset != 0 ) {
            s << '(' << hex;
            if (offset < 0) {
                s << '-' << -offset;
            } else {
                s << offset;
            }
            s << ')';
        }
        return s.str();
    }

    void setReference(CmdReference *_ref) {
        ref = _ref;
    }

    bool hasReference() {
        return ref != NULL;
    }

    bool isExtended() {
        return ref ? (getValue() > 0x7FFF) : false;
    }

    virtual uint16_t getValue() override {
        assert( ref );
        return ref->getValue() + offset;
    }

    void getExtendedValue( uint16_t& val, uint16_t& inv ) {
        val = ~getValue();
        inv = d16::INV;
    }

    CmdReference *ref;
    int16_t offset;
};

class CmdKeyword : public CmdBase {
public:

    CmdKeyword( d16::OPCODES opc ) {
        lineno = yylineno;
        type = CmdBase::Keyword;
        name = getString();
        value = opc;
    }

    string getMnemonic() {
        string s;
        d16::OPCODES opcode = static_cast<d16::OPCODES>(value);
        if( d16::mapOpcodes.find(opcode) == d16::mapOpcodes.end() ) {
            uint8_t idx = value & 0xF;
            opcode = static_cast<d16::OPCODES>(value & 0xFFF0);
            s = '(' + to_string(idx) + ')';
        }
        return d16::mapOpcodes[opcode] + s;
    }

    virtual string getString() override {
        return getMnemonic();
    }

};

class CmdDw : public CmdBase {
public:
    CmdDw() {
        lineno = yylineno;
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

    uint16_t getSize() {
        uint16_t size = 0;
        for( auto it: payload ) {
            if( it.type == dwPayload::Number || it.type == dwPayload::Ident ) {
                size++;
            } else {
                size += (it.literal.length()+1) / 2;
            }
        }
        return size;
    }

    list<dwPayload> payload;
};

void addIdentifier(string name, int16_t offset);
void addNumber(uint16_t val);
void addLabel(string name);
void addKeyword(d16::OPCODES opcode);
void addOrg( uint16_t addr );
void addDs( uint16_t size );
void addEqu( string name, uint16_t val );
void addDw();
void addPick(uint8_t idx);
void addSwap(uint8_t idx);

#endif


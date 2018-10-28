#ifndef __OPCODES_H
#define __OPCODES_H

#include <string>
#include <map>
#include <stdint.h>

namespace d16 {

enum Opcode {
    DUP     = 0xA110,
    SWAP    = 0x80A0,
    DROP    = 0xC120,
    JMPZ    = 0xE650,
    JMPNZ   = 0xFFFF, //TBD
    JMPL    = 0xE750,
    JMP     = 0xC150,
    CALL    = 0xC190,
    BRAZ    = 0xE690,
    BRAL    = 0xE790,
    RET     = 0x9050,
    PUSHRS  = 0xC100,
    DROPRS  = 0x9120,
    POPRS   = 0xB010,
    LOAD    = 0x8420,
    STORE   = 0xE860,
    PUSHPC  = 0xA210,
    PUSHSP  = 0xA310,
    POPSP   = 0x8140,
    ADD     = 0xC530,
    ADC     = 0x8581,
    SUB     = 0xC538,
    SBC     = 0x8589,
    AND     = 0xC532,
    OR      = 0xC533,
    XOR     = 0xC534,
    INV     = 0x8525,
    LSL     = 0xC536,
    LSR     = 0xC537,
};

extern std::map<Opcode, std::string> mapOpcodes;

}

#endif

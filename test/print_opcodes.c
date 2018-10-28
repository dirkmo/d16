#include <stdio.h>
#include <stdint.h>
#include "../include/d16.h"

uint16_t opcodes[] = {
    DUP,
    SWAP,
    DROP,
    JMPZ,
    JMPNZ,
    JMPL,
    JMP,
    CALL,
    BRAZ,
    BRAL,
    RET,
    PUSHRS,
    DROPRS,
    POPRS,
    LOAD,
    STORE,
    PUSHPC,
    PUSHSP,
    POPSP,
    ADD,
    ADC,
    SUB,
    SBC,
    AND,
    OR,
    XOR,
    INV,
    LSL,
    LSR
};

const char* names[] = {
    "DUP",
    "SWAP",
    "DROP",
    "JMPZ",
    "JMPNZ",
    "JMPL",
    "JMP",
    "CALL",
    "BRAZ",
    "BRAL",
    "RET",
    "PUSHRS",
    "DROPRS",
    "POPRS",
    "LOAD",
    "STORE",
    "PUSHPC",
    "PUSHSP",
    "POPSP",
    "ADD",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "OR",
    "XOR",
    "INV",
    "LSL",
    "LSR"
};

int main() {
    for( int i=0; i<sizeof(opcodes)/sizeof(opcodes[0]); i++) {
        printf("\"%s\": 0x%04X, ", names[i], opcodes[i] );
    }
    printf("\n");
    return 0;
}



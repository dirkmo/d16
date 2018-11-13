#include "opcodes.h"

namespace d16 {

std::map<OPCODES, std::string> mapOpcodes = {
    { DUP,       "DUP" },
    { SWAP,     "SWAP" },
    { PICK,     "PICK" },
    { DROP,     "DROP" },
    { JMPZ,     "JMPZ" },
    { JMPNZ,   "JMPNZ" },
    { JMPL,     "JMPL" },
    { JMP,       "JMP" },
    { BRAZ,     "BRAZ" },
    { BRANZ,   "BRANZ" },
    { BRAL,     "BRAL" },
    { CALL,     "CALL" },
    { RET,       "RET" },
    { PUSHRS, "PUSHRS" },
    { DROPRS, "DROPRS" },
    { POPRS,   "POPRS" },
    { LOAD,     "LOAD" },
    { STORE,   "STORE" },
    { PUSHPC, "PUSHPC" },
    { PUSHSP, "PUSHSP" },
    { POPSP,   "POPSP" },
    { ADD,       "ADD" },
    { ADC,       "ADC" },
    { SUB,       "SUB" },
    { SBC,       "SBC" },
    { AND,       "AND" },
    { OR,         "OR" },
    { XOR,       "XOR" },
    { INV,       "INV" },
    { LSL,       "LSL" },
    { LSR,       "LSR" },
};

}

#include "d16.h"
#include "opcodes.h"

#include <map>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

map<OPCODES, string> mapOpcodes = {
    { DUP,       "DUP"      },
    { SWAP,      "SWAP"     },
    { DROP,      "DROP"     },
    { JMPZ,      "JMPZ"     },
    { JMPNZ,     "JMPNZ"    },
    { JMPL,      "JMPL"     },
    { JMP,       "JMP"      },
    { BRAZ,      "BRAZ"     },
    { BRANZ,     "BRANZ"    },
    { BRAL,      "BRAL"     },
    { CALL,      "CALL"     },
    { RET,       "RET"      },
    { PUSHRS,    "PUSHRS"   },
    { DROPRS,    "DROPRS"   },
    { POPRS,     "POPRS"    },
    { LOAD,      "LOAD"     },
    { STORE,     "STORE"    },
    { PUSHPC,    "PUSHPC"   },
    { PUSHSP,    "PUSHSP"   },
    { POPSP,     "POPSP"    },
    { ADD,       "ADD"      },
    { ADC,       "ADC"      },
    { SUB,       "SUB"      },
    { SBC,       "SBC"      },
    { AND,       "AND"      },
    { OR,        "OR"       },
    { XOR,       "XOR"      },
    { INV,       "INV"      },
    { LSL,       "LSL"      },
    { LSR,       "LSR"      },
    { HALT,      "HALT"     },
    { (OPCODES)(PICK |  0), "PICK(0)"  },
    { (OPCODES)(PICK |  1), "PICK(1)"  },
    { (OPCODES)(PICK |  2), "PICK(2)"  },
    { (OPCODES)(PICK |  3), "PICK(3)"  },
    { (OPCODES)(PICK |  4), "PICK(4)"  },
    { (OPCODES)(PICK |  5), "PICK(5)"  },
    { (OPCODES)(PICK |  6), "PICK(6)"  },
    { (OPCODES)(PICK |  7), "PICK(7)"  },
    { (OPCODES)(PICK |  8), "PICK(8)"  },
    { (OPCODES)(PICK |  9), "PICK(9)"  },
    { (OPCODES)(PICK | 10), "PICK(10)" },
    { (OPCODES)(PICK | 11), "PICK(11)" },
    { (OPCODES)(PICK | 12), "PICK(12)" },
    { (OPCODES)(PICK | 13), "PICK(13)" },
    { (OPCODES)(PICK | 14), "PICK(14)" },
    { (OPCODES)(PICK | 15), "PICK(15)" },
};


string mnemonic(uint16_t opcode) {
    string s;
    OPCODES op = static_cast<OPCODES>(opcode);
    if( mapOpcodes.find(op) != mapOpcodes.end() ) {
        s = mapOpcodes[op];
    } else {
        stringstream ss;
        ss << std::hex << opcode;
        s = ss.str();
    }
    return s;
}

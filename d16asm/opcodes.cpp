#include "opcodes.h"

namespace d16 {

const opcodes_t opcodes[] = {
    { "DUP",    DUP    },
    { "SWAP",   SWAP   },
    { "DROP",   DROP   },
    { "JMPZ",   JMPZ   },
    { "JMPL",   JMPL   },
    { "JMP",    JMP    },
    { "CALL",   CALL   },
    { "BRAZ",   BRAZ   },
    { "BRAL",   BRAL   },
    { "RET",    RET    },
    { "PUSHRS", PUSHRS },
    { "DROPRS", DROPRS },
    { "POPRS",  POPRS  },
    { "LOAD",   LOAD   },
    { "STORE",  STORE  },
    { "PUSHPC", PUSHPC },
    { "PUSHSP", PUSHSP },
    { "POPSP",  POPSP  },
    { "ADD",    ADD    },
    { "ADC",    ADC    },
    { "SUB",    SUB    },
    { "SBC",    SBC    },
    { "AND",    AND    },
    { "OR",     OR     },
    { "XOR",    XOR    },
    { "INV",    INV    },
    { "LSL",    LSL    },
    { "LSR",    LSR    },
};

}

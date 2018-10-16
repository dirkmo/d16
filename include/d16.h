#ifndef __D16_H
#define __D16_H

enum CONSTANTS {
    OP          = 1 << 15,
    // data stack pointer ops
    DSP_NONE    = 0 << 13,
    DSP_INC     = 1 << 13,
    DSP_DEC     = 2 << 13,
    DSP_DEC2    = 3 << 13,
    // return stack pointer ops
    RSP_NONE    = 0 << 12,
    RSP_DEC     = 1 << 12,
    // src: bus driver
    SRC_RTOS    = 0 << 8,
    SRC_DTOS    = 1 << 8,
    SRC_PC      = 2 << 8,
    SRC_DSP     = 3 << 8,
    SRC_MEM     = 4 << 8,
    SRC_ALU     = 5 << 8,
    SRC_JZ      = 6 << 8, // conditional jump if zero
    SRC_JN      = 7 << 8, // conditional jump if less than zero
    SRC_DNOS    = 8 << 8,

    // dst: destination register
    DST_RS      = 0 << 4, // push return stack element
    DST_DS      = 1 << 4, // push data stack element
    DST_DS1     = 2 << 4, // set TOS (for eg. LOAD)
    DST_DS2     = 3 << 4, // set NOS (for eg. ADD)
    DST_DSP     = 4 << 4, // set data stack pointer
    DST_PC      = 5 << 4, // set PC
    DST_MEM     = 6 << 4, // write memory
    DST_RSP     = 7 << 4, // set return stack pointer
    DST_DS12    = 8 << 4, // set TOS, NOS (for NOS=alu, TOS=carry)
    DST_PC_RS   = 9 << 4, // push(R, pc), rs++, set 
    DST_T_N     = 10 << 4, // swap N,T
    // alu
    ALU_ADD     = 0 << 0,
    ALU_ADC     = 1 << 0,
    ALU_AND     = 2 << 0,
    ALU_OR      = 3 << 0,
    ALU_XOR     = 4 << 0,
    ALU_INV     = 5 << 0,
    ALU_LSL     = 6 << 0,
    ALU_LSR     = 7 << 0,
    ALU_SUB     = 8 << 0,
    ALU_SBC     = 9 << 0,
};

/*
[15] | [14-0]
 0   | val#

[15] | [14-13] | [12] | [11-8] | [7-4] | [3-0]
 1   |   dsp   |  rsp |  src   | dst   |  alu
 */

enum OPCODES {
    DUP    = OP | DSP_INC  | SRC_DTOS | DST_DS, // (n -- n n)
    SWAP   = OP | DST_T_N,
    DROP   = OP | DSP_DEC  | SRC_DTOS | DST_DS1, // (n -- )
 
    JMPZ   = OP | DSP_DEC2 | SRC_JZ   | DST_PC, // (a z -- )
    JMPL   = OP | DSP_DEC2 | SRC_JN   | DST_PC, // (a n -- )
 
    CALL   = OP | DSP_DEC  | SRC_DTOS | DST_PC_RS, // (a -- )
     
    RET    = OP | RSP_DEC  | SRC_RTOS | DST_PC, // ( -- )

    PUSHRS = OP | DSP_DEC  | SRC_DTOS | DST_RS, // (n -- )
    DROPRS = OP | SRC_DTOS | DST_DS1  | RSP_DEC, // ( -- )
    POPRS  = OP | RSP_DEC  | DSP_INC  | SRC_RTOS | DST_DS, // ( -- n)
    
    LOAD   = OP | DST_DS1  | SRC_MEM, // ( a -- n )
    STORE  = OP | DST_MEM  | SRC_DNOS | DSP_DEC2, // ( n a -- )
    
    PUSHPC = OP | SRC_PC   | DST_DS   | DSP_INC, // ( -- pc )
    PUSHSP = OP | SRC_DSP  | DST_DS   | DSP_INC, // ( -- dsp )
    POPSP  = OP | SRC_DTOS | DST_DSP, // ( dsp -- )

    ADD    = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_ADD, // (n n -- n)
    ADC    = OP | SRC_ALU  | DST_DS12 | ALU_ADC, // (n n -- n c)
    SUB    = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_SUB, // (n n -- n)
    SBC    = OP | SRC_ALU  | DST_DS12 | ALU_SBC, // (n n -- n c)
    AND    = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_AND, // (n n -- n),
    OR     = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_OR, // (n n -- n),
    XOR    = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_XOR,
    INV    = OP | SRC_ALU  | DST_DS1  | ALU_INV, // (n -- n),
    LSL    = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_LSL, // (n n -- n),
    LSR    = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_LSR, // (n n -- n),

    HALT = 0xFFFF
};

#endif

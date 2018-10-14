#ifndef __D16_H
#define __D16_H

enum CONSTANTS {
    OP          = 1 << 15,
    // conditions
    COND_NONE   = 0 << 13,
    COND_ZERO   = 1 << 13,
    COND_NEG    = 2 << 13,
    COND_CARRY  = 3 << 13,
    // data stack pointer ops
    DSP_NONE    = 0 << 11,
    DSP_INC     = 1 << 11,
    DSP_DEC     = 2 << 11,
    DSP_DEC2    = 3 << 11,
    // return stack pointer ops
    RSP_NONE    = 0 << 10,
    RSP_DEC     = 1 << 10,
    // src: bus driver
    SRC_RTOS    = 0 << 7,
    SRC_DTOS    = 1 << 7,
    SRC_PC      = 2 << 7,
    SRC_DSP     = 3 << 7,
    SRC_MEM     = 4 << 7,
    SRC_ALU     = 5 << 7,
    // dst: destination register
    DST_RS      = 0 << 4, // push return stack element
    DST_DS      = 1 << 4, // push data stack element
    DST_DS1     = 2 << 4, // set TOS (for eg. LOAD)
    DST_DS2     = 3 << 4, // set NOS (for eg. ADD)
    DST_DSP     = 4 << 4, // set data stack pointer
    DST_PC      = 5 << 4, // set PC
    DST_MEM     = 6 << 4, // write memory
    DST_RSP     = 7 << 4, // set return stack pointer
    // alu
    ALU_ADD     = 0 << 0,
    ALU_ADC     = 1 << 0,
    ALU_AND     = 2 << 0,
    ALU_OR      = 3 << 0,
    ALU_XOR     = 4 << 0,
    ALU_INV     = 5 << 0,
    ALU_LSL     = 6 << 0,
    ALU_LSR     = 7 << 0,
};

/*
[15] | [14-0]
 0   | val#

[15] | [14-13] | [12-11] | [10] | [9-7] | [6-4] | [3-0]
 1   |  cond   |   dsp   |  rsp |  src  |  dst  |  alu
 */

enum OPCODES {
    DUP   = OP | COND_NONE  | DSP_INC | SRC_DTOS | DST_DS,
    DUPZ  = OP | COND_ZERO  | DSP_INC | SRC_DTOS | DST_DS,
    DUPN  = OP | COND_NEG   | DSP_INC | SRC_DTOS | DST_DS,
    DUPC  = OP | COND_CARRY | DSP_INC | SRC_DTOS | DST_DS,

    ADD   = OP | COND_NONE  | DSP_DEC | SRC_ALU  | DST_DS2 | ALU_ADD,
    ADDZ  = OP | COND_ZERO  | DSP_DEC | SRC_ALU  | DST_DS2 | ALU_ADD,
    ADDN  = OP | COND_NEG   | DSP_DEC | SRC_ALU  | DST_DS2 | ALU_ADD,
    ADDC  = OP | COND_CARRY | DSP_DEC | SRC_ALU  | DST_DS2 | ALU_ADD,



    HALT = 0xFFFF
};

#endif

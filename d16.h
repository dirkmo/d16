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
    SRC_RTOS    = 0 << 9,
    SRC_DTOS    = 1 << 9,
    SRC_PC      = 2 << 9,
    SRC_DSP     = 3 << 9,
    SRC_MEM     = 4 << 9,
    SRC_ALU     = 5 << 9,
    SRC_JZ      = 6 << 9, // conditional jump if zero
    SRC_JN      = 7 << 9, // conditional jump if less than zero

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

[15] | [14-13] | [12] | [11-9] |   8   | [7-4] | [3-0]
 1   |   dsp   |  rsp |  src   |unused | dst   |  alu
 */

enum OPCODES {
    DUP   = OP | DSP_INC  | SRC_DTOS | DST_DS, // (n -- nn)
    ADD   = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_ADD, // (n n -- n)

    SUB   = OP | DSP_DEC  | SRC_ALU  | DST_DS2  | ALU_SUB, // (n n -- n)

    JMPZ  = OP | DSP_DEC2 | SRC_JZ   | DST_PC, // (a z -- )
    JMPL  = OP | DSP_DEC2 | SRC_JN   | DST_PC, // (a n -- )

    HALT = 0xFFFF
};

#endif

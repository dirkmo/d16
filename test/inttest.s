.org 0
reset: main jmp
; int vector table
int1: isr1 jmp
int2: isr2 jmp
int3: isr3 jmp
int4: isr4 jmp
int5: isr5 jmp
int6: isr6 jmp
int7: isr7 jmp
int8: isr8 jmp

main: 1 2 add drop main jmp

.org 0x40

isr1: 1 drop ret
isr2: 2 drop ret
isr3: 3 drop ret
isr4: 4 drop ret
isr5: 5 drop ret
isr6: 6 drop ret
isr7: 7 drop ret
isr8: .dw 0xFFFF

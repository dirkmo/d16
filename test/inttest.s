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

main: main jmp

isr1: ret
isr2: ret
isr3: ret
isr4: ret
isr5: ret
isr6: ret
isr7: ret
isr8: .dw 0xFFFF

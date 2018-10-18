; d16 assembly
.org 0
.equ Test 0xABCD

start:

0
100
56163
0x1234
123
add
DROP

loop:
loop jmp

data:
    .dw 0, 0xFFFF, 'a'
    .dw "Hallo"
    .ds 100

addr:.dw start

data_end:

.equ len 123

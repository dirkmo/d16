; d16 assembly
.org 0

start:

0
100
0x1234
-100
add
DROP

loop:
loop jmp

data:
    .dw 0, 0xFFFF, 'a'
    .dw "Hallo"
    .ds 100
data_end:

.equ len data_end - data
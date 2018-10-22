.org 0
.equ ledport 0x7000

1

loop:   dup set braz
        dup ledport store
        1 lsl
        loop jmp

set:    drop 1
        ret

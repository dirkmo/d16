.org 0
.equ ledport 0x7000

.equ ram 0x1000

1 ram store

loop:   ram load
        dup set braz
        dup ledport store
        1 lsl
        ram store
        loop jmp

set:    drop 1
        ret

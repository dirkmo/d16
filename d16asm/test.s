; uart test
.equ uart 0xFF00

.macro write
\1 2 add 3 sub
dup drop
.endmacro

.org 0

msg print call

.dw 0xFFFF ; sim end
dummy
print:
; TOS = string addr
        ; msg
        dup load ; msg cc
        dup done jmpz ; msg cc
        dup  ; msg cc cc
        uart store ; msg cc
        8 lsr ; msg 0c
dummy:  dup done jmpz ; msg 0c
        uart store ; msg
        1 add ; msg+1
        print jmp

done:   ret
        pick(0)
        pick(15)
        pick(1)
        swap(0)
        swap(1)
        dummy(1)
        dummy(-1)

.ds 10

msg: .dw 1,"Hallo Welt!",0, uart, done

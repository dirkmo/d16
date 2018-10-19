; uart test
.equ uart 0xF000

.org 0

msg print call

0xFFFF ; sim end


print:
; TOS = string addr
        ; msg
        dup load ; msg cc
        dup done jmpz ; msg cc
        dup  ; msg cc cc
        uart store ; msg cc
        8 lsr ; msg 0c
        dup done jmpz ; msg 0c
        uart store ; msg
        1 add ; msg+1
        print jmp

done:   ret


msg: .dw "Hallo Welt!",0

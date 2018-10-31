; uart test for simulator

.equ uart 0xFF00

.org 0

msg print call

.dw 0xFFFF ; sim end

print:  dup load ; msg cc
        dup done jmpz ; msg cc
        dup uart store ; msg cc
        8 lsr ; msg 0c
        dup done jmpz ; msg 0c
        uart store ; msg
        1 add ; msg+1
        print jmp
done:   ret

msg: .dw "Hallo Welt!",0

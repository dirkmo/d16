; uart test for simulator

.equ uart_rx 0xFF00
.equ uart_tx 0xFF00
.equ uart_stat 0xff01

.org 0

msg print call

.dw 0xFFFF ; sim end

print:  dup load ; msg cc
        dup done jmpz ; msg cc
        uart_ready_to_send call
        dup uart_tx store ; msg cc
        8 lsr ; msg 0c
        dup done jmpz ; msg 0c
        uart_ready_to_send call
        uart_tx store ; msg
        1 add ; msg+1
        print jmp
done:   ret

uart_ready_to_send:
        uart_stat load
        4 and
        uart_ready_to_send jmpnz
        ret

msg: .dw "D16 Simulator running.",0

; uart test for simulator

.equ uart_rx 0xFF00
.equ uart_tx 0xFF00
.equ uart_stat 0xff01

.org 0

; print msg
msg printstr call

; receive string
buf receivestr call

.dw 0xFFFF ; sim end

printstr:  dup load ; msg cc
        dup done jmpz ; msg cc
        uart_ready_to_send call
        dup uart_tx store ; msg cc
        8 lsr ; msg 0c
        dup done jmpz ; msg 0c
        uart_ready_to_send call
        uart_tx store ; msg
        1 add ; msg+1
        printstr jmp
done:   ret

uart_ready_to_send:
        uart_stat load
        4 and
        uart_ready_to_send jmpnz
        ret

receivestr: ; TOS: buf address
        bufidx store ; save buf address to bufidx
        receivebyte call ; receive byte
        dup 13 sub recdone jmpz ; if byte == 13 then goto recdone
        bufidx load store
        bufidx load 1 add
        jmp receivestr
recdone:
        0 bufidx load 1 add store
        ret

receivebyte: ; ( -- n )
        ; get DA bit
        uart_stat load 1 and
        receivebyte jmpz ; loop until nonzero
        uart_rx load ; read byte from uart
        ret



msg: .dw "D16 Simulator running.",0
bufidx: .dw 0
buf: .ds 128

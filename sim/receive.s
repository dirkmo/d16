.equ uart_rx 0xFF00
.equ uart_tx 0xFF00
.equ uart_stat 0xff01

.org 0

loop:
        receivebyte call
        
        dup send jmpz
        ; add byte to fifo
        last load store
        
        ; last = (last+1) & 0x1F
        last load 1 add 0x1F and last store
        
        ; if first == last goto loop
        first load last load sub loop jmpz

send:   first load load ; get char from fifo
        dup loop jmpz ; if char==zero goto loop
        sendbyte call
        
        ; first = (first + 1) & 0x1F
        first load 1 add 0x1F and first store

        loop jmp

receivebyte: ; ( -- n )
        ; get DA bit
        uart_stat load 1 and
        dup x1 jmpz
        uart_rx load ; read byte from uart
x1:     ret

sendbyte: ; ( n -- )
        ; get tx active bit
        uart_stat load 4 and
        sendbyte jmpnz
        uart_tx store
        ret

first: buf
last: buf
buf: .ds 32

.dw 0xFFFF

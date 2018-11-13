.equ uart_rx 0xFF00
.equ uart_tx 0xFF00
.equ uart_stat 0xff01

.org 0

loop:
        receivebyte call
        
        ; if rec!=0 push()
        push branz

        pop call
        loop1 jmpz ; if popresult == 0 goto loop1

        sendbyte call
        loop jmp

loop1:  drop ; drop 0 char
        loop jmp

receivebyte: ; ( -- n )
        ; receive byte from uart. returns 0 if no byte avail
        ; get DA bit
        uart_stat load 1 and
        x1 jmpz
        uart_rx load ; read byte from uart
        1 ret
x1:     0 ret

sendbyte: ; ( n -- )
        ; get tx active bit
        uart_stat load 4 and
        sendbyte jmpnz
        uart_tx store
        ret

push:   ; ( n -- )
        ; fifo is full when last + 1 == first
        first load last load 1 add sub push1 jmpz
        ; push to fifo
        last load buf add store
        ; last = (last+1) & 0x1F
        last load 1 add 0x1F and last store
push1:  ret

pop:    ; ( -- dat success )
        ; first - last
        first load last load sub pop1 jmpnz
        ; no data avail
        0 0 ; return value: no data
        ret
pop1:   ; data avail in fifo
        first load buf add load
        ; first = (first + 1) & 0x1F
        first load 1 add 0x1F and first store
        1 ; return value: data popped
        ret


first: 0
last: 0
buf: .ds 32


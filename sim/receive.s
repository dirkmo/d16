.equ uart_rx 0xFF00
.equ uart_tx 0xFF00
.equ uart_stat 0xff01

.org 0

loop:
        receivebyte call
        
        ; if rec!=0 push()
        fifopush branz

        fifopop call
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

fifopush: ; ( d a -- ret )
        ; get first idx
        dup 1 add load ; d a first
        ; get last idx
        pick(1) 2 add load ; d a first last
        ; last + 1
        1 add ; d a first last+1
        sub l1 jmpz ; if (first == last+1) goto l1
        dup 1 add load ; d a last
        pick(2) ; d a last d
        swap(1) ; d a d last
        store ; d a
        dup 2 add dup load ; d a a+2 last
        1 add 0x1F and; d a a+2 (last+1)&0x1F
        swap(1) ; d a (last+1)&0x1F a+2
        store ; d a
        drop drop
        1 ret
l1:     drop drop
        0 ret

fifopop: ; ( a -- d ret )
        ; get first idx
        dup 1 add load ; a first
        ; get last idx
        pick(1) 2 add load ; a first last
        sub l2 jmpz ; a
        dup 1 add load ; a first
        load ; a d
        swap(1) ; d a
        1 add dup load ; d a+1 first
        swap(1) ; d first a+1
        store ; d
        1 ret ; d 1
l2:     drop
        0 0 ret ; 0 0

rxfifo: 0 0 .ds 32


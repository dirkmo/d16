; uart test
.equ uart 0xF000

.org 0

msg
print
call

.dw 0xFFFF ; sim end

print:
; TOS = string addr
        ; msg
        dup  ; msg msg
        load ; msg cc
        dup  ; msg cc cc
        done ; msg cc cc done
        swap ; msg cc done cc
        jmpz ; msg cc
        uart
        store

done:   ret


msg: .dw "Hallo Welt!",0

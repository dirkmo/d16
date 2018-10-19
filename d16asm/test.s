; uart test
.equ uart 0xF000

.org 0


print:
; TOS = string addr
        dup
        load
        dup
        jmpz done
        uart
        store
done:   ret


msg: .dw "Hallo Welt!",0

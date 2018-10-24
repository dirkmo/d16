.PHONY: clean all

all: d16asm

d16asm: d16asm.l d16asm.y d16asm.h
	bison -d d16asm.y
	flex -o d16asm.lex.c d16asm.l
	gcc -o $@ d16asm.tab.c d16asm.lex.c

clean:
	rm -f d16asm.lex.c d16asm.tab.c d16asm.tab.h



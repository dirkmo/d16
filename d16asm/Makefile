.PHONY: clean all

CPPFLAGS=-std=c++14 -Wall

all: d16asm

d16asm: d16asm.lpp d16asm.ypp d16asm.h
	bison -d d16asm.ypp
	flex -o d16asm.lex.cpp d16asm.lpp
	g++ $(CPPFLAGS) -Wno-write-strings -c d16asm.tab.cpp -o d16asm.tab.o
	g++ $(CPPFLAGS) -Wno-write-strings -c d16asm.lex.cpp -o d16asm.lex.o
	g++ $(CPPFLAGS) d16asm.cpp d16asm.tab.o d16asm.lex.o -o $@
	

clean:
	rm -f *.o d16asm.lex.cpp d16asm.tab.cpp d16asm.tab.hpp d16asm



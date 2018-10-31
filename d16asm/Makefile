.PHONY: clean all

CPPFLAGS=-std=c++14 -Wall -g3
LEXFLAGS=-Wno-sign-compare -Wno-write-strings -Wno-unused-function -Wno-deprecated-register

all: d16asm

assemble.o: assemble.cpp assemble.h
	g++ $(CPPFLAGS) -c assemble.cpp -o assemble.o

opcodes.o: opcodes.cpp opcodes.h
	g++ $(CPPFLAGS) -c opcodes.cpp -o opcodes.o

d16asm: d16asm.lpp d16asm.ypp d16asm.h d16asm.cpp assemble.o opcodes.o
	bison -d d16asm.ypp
	flex -o d16asm.lex.cpp d16asm.lpp
	g++ $(CPPFLAGS) -Wno-write-strings -c d16asm.tab.cpp -o d16asm.tab.o
	g++ $(CPPFLAGS) $(LEXFLAGS) -c d16asm.lex.cpp -o d16asm.lex.o
	
	g++ $(CPPFLAGS) opcodes.o assemble.o d16asm.tab.o d16asm.lex.o d16asm.cpp -o $@
	

clean:
	rm -f *.o d16asm.lex.cpp d16asm.tab.cpp d16asm.tab.hpp d16asm



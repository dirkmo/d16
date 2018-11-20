.PHONY: clean all

CPPFLAGS=-std=c++14 -Wall -g3
LEXFLAGS=-Wno-sign-compare -Wno-write-strings -Wno-unused-function -Wno-deprecated-register

CPPFILES=d16asm.lex.cpp assemble.cpp d16asm.cpp opcodes.cpp d16asm.tab.cpp
OBJFILES := $(CPPFILES:.cpp=.o)

.cpp.o:
	g++ $(CPPFLAGS) $(LEXFLAGS) -c $< -o $@

all: d16asm

d16asm.tab.hpp: d16asm.ypp
	bison -d d16asm.ypp

d16asm.lex.cpp: d16asm.lpp d16asm.tab.hpp
	flex -o d16asm.lex.cpp d16asm.lpp

d16asm: d16asm.lex.cpp $(OBJFILES)
	g++ $(CPPFLAGS) $(OBJFILES) -o $@

clean:
	rm -f $(OBJFILES) d16asm.lex.cpp d16asm.tab.cpp d16asm.tab.hpp d16asm

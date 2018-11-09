.PHONY: all clean sim wave

UNAME := $(shell uname -s)

VERILATOR_OUTPUTDIR = verilator

INCDIR=-I$(VERILATOR_OUTPUTDIR) -I../verilog -I../verilog/uart -I/usr/share/verilator/include 

VFLAGS = -CFLAGS -std=c++11 -Wall -trace -cc --exe $(INCDIR) --Mdir $(VERILATOR_OUTPUTDIR)
GTKWAVE := gtkwave
ifeq ($(UNAME),Darwin)
VFLAGS += --compiler clang
GTKWAVE := /Applications/gtkwave.app/Contents/MacOS/gtkwave-bin
endif

CFLAGS=-Wall -std=c++11

CPPFILES=client.cpp debug.cpp
OBJFILES=$(CPPFILES:.cpp=.o)

all: server d16sim

.cpp.o:
	g++ $(CFLAGS) $(INCDIR) -c $< -o $@

verilator: sim.cpp
	verilator $(VFLAGS) top.v ../client.o ../debug.o sim.cpp


d16sim: verilator $(OBJFILES)
	make -C $(VERILATOR_OUTPUTDIR) -j4 -f Vtop.mk

server: server.cpp server.h
	g++ -std=c++11 -Wall -g3 server.cpp -lpthread -o server

sim: d16sim
	$(VERILATOR_OUTPUTDIR)/Vtop -d

wave: sim
	gtkwave trace.vcd &

clean:
	rm -f $(VERILATOR_OUTPUTDIR)/*
	-rm -r $(VERILATOR_OUTPUTDIR)

.PHONY: all clean sim wave

UNAME := $(shell uname -s)

VFLAGS = -Wall -trace -cc --exe -I../verilog -I../verilog/uart --Mdir $@
GTKWAVE := gtkwave
ifeq ($(UNAME),Darwin)
VFLAGS += --compiler clang
GTKWAVE := /Applications/gtkwave.app/Contents/MacOS/gtkwave-bin
endif

all: d16sim

client.o: client.cpp client.h
	g++ -Wall -c client.cpp -o client.o

d16sim: client.o
	verilator $(VFLAGS) top.v ../client.o sim.cpp
	cd d16sim/ && make -j4 -f Vtop.mk

server: server.cpp server.h
	g++ -Wall server.cpp -o server

sim: d16sim server
	d16sim/Vtop

wave: sim
	gtkwave trace.vcd &

clean:
	rm -f d16sim/*
	-rm -r d16sim

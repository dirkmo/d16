.PHONY: all clean sim wave

UNAME := $(shell uname -s)

VFLAGS = -CFLAGS -std=c++11 -Wall -trace -cc --exe -I../verilog -I../verilog/uart --Mdir $@
GTKWAVE := gtkwave
ifeq ($(UNAME),Darwin)
VFLAGS += --compiler clang
GTKWAVE := /Applications/gtkwave.app/Contents/MacOS/gtkwave-bin
endif

CFLAGS=-Wall -I/usr/share/verilator/include

all: d16sim

d16sim:
	verilator $(VFLAGS) top.v ../client.o ../debug.o sim.cpp
	g++ -Wall -I/usr/share/verilator/include -c debug.cpp -o debug.o
	g++ $(CFLAGS) -c client.cpp -o client.o
	cd d16sim/ && make -j4 -f Vtop.mk

server: server.cpp server.h
	g++ -std=c++11 -Wall -g3 server.cpp -lpthread -o server

sim: d16sim server
	d16sim/Vtop -d

wave: sim
	gtkwave trace.vcd &

clean:
	rm -f d16sim/*
	-rm -r d16sim

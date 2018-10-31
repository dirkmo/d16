.PHONY: all clean sim wave

UNAME := $(shell uname -s)

VFLAGS = -Wall -trace -cc --exe -I../verilog -I../verilog/uart --Mdir $@
GTKWAVE := gtkwave
ifeq ($(UNAME),Darwin)
VFLAGS += --compiler clang
GTKWAVE := /Applications/gtkwave.app/Contents/MacOS/gtkwave-bin
endif

all: d16sim

d16sim:
	verilator $(VFLAGS) top.v sim.cpp
	cd d16sim/ && make -j4 -f Vtop.mk

sim: d16sim
	d16sim/Vd16sim

wave: sim
	gtkwave trace.vcd &

clean:
	rm -f d16sim/*
	-rm -r d16sim

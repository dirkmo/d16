.PHONY: all clean sim wave

UNAME := $(shell uname -s)

VFLAGS = -Wall -trace -cc --exe --Mdir $@
GTKWAVE := gtkwave
ifeq ($(UNAME),Darwin)
VFLAGS += --compiler clang
GTKWAVE := /Applications/gtkwave.app/Contents/MacOS/gtkwave-bin
endif

all: d16

d16:
	verilator $(VFLAGS) ../verilog/d16.v sim.cpp
	cd d16/ && make -j4 -f Vd16.mk

sim: d16
	d16/Vd16

wave: sim
	gtkwave trace.vcd &

clean:
	rm -f d16/*
	-rm -r d16

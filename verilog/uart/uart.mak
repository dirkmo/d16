.PHONY: all clean sim wave

UNAME := $(shell uname -s)

VFLAGS = -Wall -trace -cc --exe --Mdir $@
GTKWAVE := gtkwave
ifeq ($(UNAME),Darwin)
VFLAGS += --compiler clang
GTKWAVE := /Applications/gtkwave.app/Contents/MacOS/gtkwave-bin
endif

all: uart


uart:
	verilator $(VFLAGS) uart.v uart_test.cpp
	cd uart/ && make -j4 -f Vuart.mk

sim: uart
	uart/Vuart

wave: sim
	gtkwave trace.vcd &

clean:
	rm -f uart/*
	-rm -r uart

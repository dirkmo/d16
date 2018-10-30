.PHONY: all clean sim wave

UNAME := $(shell uname -s)

VFLAGS = -Wall -trace -cc --exe --Mdir $@
GTKWAVE := gtkwave
ifeq ($(UNAME),Darwin)
VFLAGS += --compiler clang
GTKWAVE := /Applications/gtkwave.app/Contents/MacOS/gtkwave-bin
endif

all: uart_rx


uart_rx:
	verilator $(VFLAGS) uart_rx.v uart_rx_test.cpp
	cd uart_rx/ && make -j4 -f Vuart_rx.mk

sim: uart_rx
	uart_rx/Vuart_rx

wave: sim
	gtkwave trace.vcd &

clean:
	rm -f uart_rx/*
	-rm -r uart_rx

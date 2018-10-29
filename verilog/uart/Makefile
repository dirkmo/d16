.PHONY: all clean sim wave

UNAME := $(shell uname -s)

VFLAGS = -Wall -trace -cc --exe --Mdir $@
GTKWAVE := gtkwave
ifeq ($(UNAME),Darwin)
VFLAGS += --compiler clang
GTKWAVE := /Applications/gtkwave.app/Contents/MacOS/gtkwave-bin
endif

all: uart_tx


uart_tx:
	verilator $(VFLAGS) uart_tx.v uart_tx_test.cpp
	cd uart_tx/ && make -j4 -f Vuart_tx.mk

sim: uart_tx
	uart_tx/Vuart_tx

wave: sim
	gtkwave trace.vcd &

clean:
	rm -f uart_tx/*
	-rm -r uart_tx

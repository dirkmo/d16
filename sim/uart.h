#ifndef __UART_H
#define __UART_H

#include <stdint.h>
#include <stdio.h>
#include "../include/protothreads.h"

class Uart {
public:
    enum CONSTANTS {
        SYS_CLK   = 1000000,
        BAUDRATE  = 115200,
        UART_TICK = (SYS_CLK / BAUDRATE)+1,
    };

    Uart( uint8_t *_tx, const uint8_t *_rx, const uint8_t *_clk) : tx(*_tx), rx(*_rx), clk(*_clk) {
        PT_INIT(&tx_pt, NULL);
        PT_INIT(&rx_pt, NULL);
    }

    void task() {
        send();
        receive();
    }

    PT_THREAD(send()) {
        PT_BEGIN(&tx_pt);
        tx = 1;
        PT_END(&tx_pt);
    }

    PT_THREAD(receive()) {
        static int state = 0;
        static int baudcount = 0;
        PT_BEGIN(&rx_pt);
        while(1) {
            PT_WAIT_UNTIL(&rx_pt, clk);
            // clk = 1
            PT_WAIT_WHILE(&rx_pt, clk);
            // clk = 0
            baudcount = (baudcount + 1) % UART_TICK;
            switch(state) {
                case 0: // idle
                    if( rx == 0 ) {
                        baudcount = 0;
                        state = 1;
                    }
                    break;
                case 1: // start bit
                        if( baudcount >= UART_TICK / 2 ) {
                            dat_rx = 0;
                            state = rx ? 0 : 2;
                            baudcount = 0;
                        }
                        break;
                case 2 ... 9: // receive bits
                    if( baudcount == UART_TICK-1) {
                        dat_rx |= rx << (state-2);
                        state++;
                    }
                    break;
                case 10: // stop bit
                    if( baudcount == UART_TICK-1) {
                        printf("UART-RX: %c (%d)\n", dat_rx, dat_rx);
                        if( rx == 1 ) {
                        } else {
                            printf("UART-RX: ERROR receiving data.\n");
                        }
                        state = 0;
                    }
                    break;
                default: state = 0;
            }
        }
        PT_END(&rx_pt);
    }

    uint8_t& tx;
    const uint8_t& rx;
    const uint8_t& clk;

    uint8_t dat_rx;
    struct pt tx_pt;
    struct pt rx_pt;
};

#endif
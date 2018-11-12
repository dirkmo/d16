#ifndef __UART_H
#define __UART_H

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include "../include/protothreads.h"
#include "client.h"

class Uart {
public:
    enum CONSTANTS {
        SYS_CLK   = 4000000,
        BAUDRATE  = 115200,
        UART_TICK = (SYS_CLK / BAUDRATE)+1,
    };

    Uart( uint8_t *_tx, const uint8_t *_rx, const uint8_t *_clk)
    : tx(*_tx), rx(*_rx), clk(*_clk), uds("/tmp/d16sim.uds")
    {
        PT_INIT(&tx_pt, NULL);
        PT_INIT(&rx_pt, NULL);
        tx = 1;
    }

    void sendbyte(uint8_t dat) {
        vDataToSend.push_back(dat);
    }

    bool isSending() const {
        return vDataToSend.size() > 0;
    }

    void task() {
        uds.receive(vDataToSend);
        send();
        receive();
    }

private:
    PT_THREAD(send()) {
        static int state = 0;
        static int baudcount = 0;
        PT_BEGIN(&tx_pt);
        (void)PT_YIELD_FLAG;
        while(1) {
            PT_WAIT_UNTIL(&tx_pt, clk);
            // clk = 1
            PT_WAIT_WHILE(&tx_pt, clk);
            // clk = 0
            baudcount = (baudcount + 1) % UART_TICK;
            switch(state) {
                case 0: // idle
                    tx = 1;
                    if( vDataToSend.size() > 0 ) {
                        //start_tx = false;
                        dat_tx = vDataToSend.front();
                        vDataToSend.erase(vDataToSend.begin());
                        baudcount = 0;
                        state = 1;
                    }
                    break;
                case 1: // start bit
                    tx = 0;
                    if( baudcount == UART_TICK-1 ) {
                        state = 2;
                    }
                    break;
                case 2 ... 9: // send bits
                    tx = (dat_tx >> (state-2)) & 1;
                    if( baudcount == UART_TICK-1) {
                        state++;
                    }
                    break;
                case 10: // stop bit
                    tx = 1;
                    if( baudcount == UART_TICK-1) {
                        state = 0;
                    }
                    break;
                default: state = 0;
            }
        }
        PT_END(&tx_pt);
    }

    PT_THREAD(receive()) {
        static int state = 0;
        static int baudcount = 0;
        PT_BEGIN(&rx_pt);
        (void)PT_YIELD_FLAG;
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
                        std::cout << "Received: " << dat_rx << std::endl;
                        if( rx == 1 ) {
                            uds.send( { dat_rx } );
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

    uint8_t dat_rx; // receive reg
    uint8_t dat_tx; // send reg

    struct pt tx_pt;
    struct pt rx_pt;

    std::vector<uint8_t> vDataToSend;

    UDSClient uds;
};

#endif

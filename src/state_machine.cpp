#ifndef _STATEMACHINE
#define _STATEMACHINE

#include <cstdint>
#include "shift.cpp"
#include "fifo.cpp"

class StateMachine {
    private:
        // Internal Registers
        Shift OSR, ISR;
        uint8_t OSR_counter, IRS_counter;
        uint32_t X, Y, PC;

        // External References
        FIFO* TX_FIFO, RX_FIFO;
        uint32_t* progmem;

};

#endif
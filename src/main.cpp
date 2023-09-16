#include <iostream>

#include "state_machine.cpp"

static const uint16_t JMPtest_program_instructions[] = {
            //     .wrap_target
    0x0002, //  0: jmp    2                          
    0xa042, //  1: nop                               
    0xa042, //  2: nop                               
    0xa042, //  3: nop                               
    0xa042, //  4: nop                               
    0xa042, //  5: nop                               
            //     .wrap
};


int main(){

    FIFO TX = FIFO(), RX = FIFO();
    StateMachine sm = StateMachine();
    sm.connect(&TX, &RX, JMPtest_program_instructions, 0, 0);

    for(int i = 0 ; i < 5 ; i++){
        sm.print();
        sm.run();
    }

    return 0;
}
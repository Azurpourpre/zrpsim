#include <iostream>

#include "state_machine.cpp"

static const uint16_t test1_program_instructions[] = {
            //     .wrap_target
    0xe025, //  0: set    x, 5                       
    0xe050, //  1: set    y, 16                      
    0xa052, //  2: mov    y, ::y                     
    0x0042, //  3: jmp    x--, 2                     
            //     .wrap
};


int main(){

    FIFO TX = FIFO(), RX = FIFO();
    StateMachine sm = StateMachine();
    sm.connect(&TX, &RX, test1_program_instructions, 0, 0);

    for(int i = 0 ; i < 100; i++){
        sm.print();
        sm.run();
    }

    return 0;
}
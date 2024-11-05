#include <iostream>

#include "state_machine.h"
#include "gpio.h"

static const uint16_t io_test_program_instructions[] = {
    0xe024,
    0x4001,
    0x0041,
    0x8020,
    0x0000,
};

int main(){

    FIFO TX = FIFO(), RX = FIFO();
    
    VCDWriter writer = VCDWriter("out/out.vcd");
    CustomReader_GPIO reader("in/in.test.vcd");
    GPIO gpio = GPIO(&writer, NULL, 133e6);
    reader.update_fifo_state(&TX, 0);

    StateMachine sm = StateMachine(0,0,1);
    sm.connect(&TX, &RX, &gpio, io_test_program_instructions);
    sm.conf_in(5);

    for(int i = 0; i < 10000 && sm.get_pc() < 5; i++){
        reader.update_fifo_state(&TX, gpio.get_time());
        sm.run();
        gpio.next_time();
    }

    return 0;
}
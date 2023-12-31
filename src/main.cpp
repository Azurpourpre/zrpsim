#include <iostream>

#include "state_machine.cpp"
#include "gpio.cpp"

static const uint16_t io_test_program_instructions[] = {
            //     .wrap_target
    0xe020, //  0: set    x, 0                       
    0xe040, //  1: set    y, 0                       
    0xe001, //  2: set    pins, 1                    
    0xa001, //  3: mov    pins, x                    
    0xa029, //  4: mov    x, !x                      
    0xa001, //  5: mov    pins, x                    
    0xea00, //  6: set    pins, 0                [10]
    0x0002, //  7: jmp    2                          
            //     .wrap
};

int main(){

    FIFO TX = FIFO(), RX = FIFO();
    
    VCDWriter writer = VCDWriter("out/out.vcd");
    CustomReader_GPIO reader("in/in.test");
    GPIO gpio = GPIO(&writer, &reader, 133e6);
    reader.update_fifo_state(&TX, 0);

    StateMachine sm = StateMachine(0,0,1);
    sm.connect(&TX, &RX, &gpio, io_test_program_instructions);
    sm.conf_out(28,4); sm.conf_set(10,1);

    for(int i = 0; i < 100; i++){
        reader.update_fifo_state(&TX, gpio.get_time());
        sm.run();
        gpio.next_time();
    }

    return 0;
}
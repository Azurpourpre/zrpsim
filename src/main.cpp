#include <iostream>

#include "state_machine.cpp"
#include "GPIO/gpio.cpp"

static const uint16_t io_test_program_instructions[] = {
            //     .wrap_target
    0xe020, //  0: set    x, 0                       
    0xa001, //  1: mov    pins, x                    
    0xa029, //  2: mov    x, !x                      
    0xa001, //  3: mov    pins, x                    
    0x0001, //  4: jmp    1                          
            //     .wrap
};


int main(){

    FIFO TX = FIFO(), RX = FIFO();
    
    VCDWriter writer = VCDWriter("out/out.vcd");
    GPIO gpio = GPIO(&writer, 133e6);

    gpio.write_pin(0,1);
    gpio.write_pin(1,1);
    gpio.next_time();
    gpio.write_pin(0,0);
    gpio.next_time();
    gpio.next_time();

    return 0;
}
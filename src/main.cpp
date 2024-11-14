#include <iostream>
#include <cmath>

#include "state_machine.h"
#include "gpio.h"

static const uint16_t io_test_program_instructions[] = {
            //     .wrap_target
    0xe024, //  0: set    x, 4                       
    0x4001, //  1: in     pins, 1                    
    0x0041, //  2: jmp    x--, 1                     
    0x8020, //  3: push   block                      
    0x0000, //  4: jmp    0                          
            //     .wrap
};

#define CLOCK 133e6

int main(){

    FIFO TX = FIFO(), RX = FIFO();
    
    VCDWriter writer = VCDWriter("out/out.vcd");
    VCDSignal* rx_sig = writer.create_signal(32, "RX");

    CustomReader_GPIO reader("in/in.test.vcd");
    reader.update_fifo_state(&TX, 0);

    GPIO gpio = GPIO(&writer, &reader, CLOCK);

    StateMachine sm = StateMachine(0,0,1);
    sm.connect(&TX, &RX, &gpio, io_test_program_instructions);
    sm.conf_in(5);
    sm.conf_out(10,2);

    for(unsigned int i = 0; (i < UINT32_MAX) && (sm.get_pc() < 5) && (gpio.get_time() < reader.get_max_time()); i++){
        reader.update_fifo_state(&TX, gpio.get_time());
        sm.run();
        gpio.next_time();
        sm.print();
        std::cout << "GPIO : " << std::log2(gpio.get_pinState()) << std::endl;
    }
    gpio.write_time();

    delete[] rx_sig->name;
    delete rx_sig;

    return 0;
}
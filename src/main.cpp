#include <iostream>

#include "state_machine.cpp"
#include "GPIO/vcd.cpp"

static const uint16_t test1_program_instructions[] = {
            //     .wrap_target
    0xe025, //  0: set    x, 5                       
    0xe050, //  1: set    y, 16                      
    0xa052, //  2: mov    y, ::y                     
    0x0042, //  3: jmp    x--, 2                     
            //     .wrap
};


int main(){

    // FIFO TX = FIFO(), RX = FIFO();
    // StateMachine sm = StateMachine();
    // sm.connect(&TX, &RX, test1_program_instructions, 0, 0);

    // for(int i = 0 ; i < 100; i++){
    //     sm.print();
    //     sm.run();
    // }

    VCDWriter writer = VCDWriter("out/out.test");
    VCDSignal* rx = writer.create_signal(1, "RX");
    VCDSignal* tx = writer.create_signal(2, "TX");

    writer.write_header("1ns");

    for(int i = 1; i < 15; i++){
        std::string str = std::to_string(i);
        writer.write_time(str.c_str());
        writer.write_value(rx, i);
        writer.write_value(tx, i);
    }

    return 0;
}
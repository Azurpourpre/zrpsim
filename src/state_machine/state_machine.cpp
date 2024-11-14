#include "../state_machine.h"
#include <iostream>
#include <bit>

StateMachine::StateMachine(bool ISR_dir, bool OSR_dir, const unsigned int clock_divisor){
    this->OSR = new Shift(OSR_dir);
    this->ISR = new Shift(ISR_dir);

    this->OSR_counter = 0; //Default value ?
    this->ISR_counter = 0;
    this->stall_counter = 0;
    this->X = 0;
    this->Y = 0;
    this->PC = 0;
    this->clock_div = clock_divisor;

    this->TX_FIFO = nullptr;
    this->RX_FIFO = nullptr;
    this->progmem = nullptr;
    this->gpio = nullptr;
}

StateMachine::~StateMachine(){
    delete this->ISR;
    delete this->OSR;
}

void StateMachine::connect(FIFO* TX, FIFO* RX, GPIO* gpio, const instr* progmem) {
    this->TX_FIFO = TX;
    this->RX_FIFO = RX;
    this->gpio = gpio;
    this->progmem = progmem;
}

void StateMachine::run(){
    //Check stall
    if (stall_counter > 0){
        stall_counter --;
        return;
    }
    else {
        // Do the full thing

        //Fetch
        instr IR = this->progmem[this->PC];

        //Decode & Execute
        this->execute(IR);

    }
}

uint32_t StateMachine::get_pc(){
    return this->PC;
}


void StateMachine::conf_pin(unsigned int* pin_arr, size_t pin_arr_size){
    pin_mask = 0;
    for(unsigned int i = 0; i < pin_arr_size; i++){
        if(pin_arr[i] > 30){
            std::cerr << "Pin " << std::to_string(pin_arr[i]) << " does not exist....";
            return;
        }
        pin_mask = pin_mask | (1 << pin_arr[i]);
    }
}

void StateMachine::conf_in(unsigned int pin){
    this->PINCTRL_IN_BASE = pin;
}
void StateMachine::conf_jmp(unsigned int pin){
    this->EXECCTRL_JMP_PIN = pin;
}
void StateMachine::conf_out(unsigned int pin, unsigned int count){
    if(pin >= 30){
        std::cerr << "Pin " << std::to_string(pin) << " is not a valid pin...";
        return;
    }
    if(count >= 32){
        std::cerr << "Can't write more than " << std::to_string(count) << " bits...";
        return;
    }

    this->PINCTRL_OUT_BASE = pin;
    this->PINCTRL_OUT_COUNT = count;
}
void StateMachine::conf_set(unsigned int pin, unsigned int count){
    if(pin >= 30){
        std::cerr << "Pin " << std::to_string(pin) << " is not a valid pin...";
        return;
    }
    if(count >= 32){
        std::cerr << "Can't write more than " << std::to_string(count) << " bits...";
        return;
    }

    this->PINCTRL_SET_BASE = pin;
    this->PINCTRL_SET_COUNT = count;
}

// Debug
void StateMachine::print(){
    std::cout << "### NEW STEP ###" << std::endl;
    std::cout << "PC : " << this->PC << std::endl;
    std::cout << "STALL : " << (int)this->stall_counter << std::endl;
    std::cout << "X : " << std::hex << this->X << std::endl;
    std::cout << "Y : " << std::hex << this->Y << std::endl;
    std::cout << "ISR : " << std::hex << this->ISR->get() << std::endl;
    std::cout << "OSR : " << std::hex << this->OSR->get() << std::endl;
    std::cout << "TX FIFO : "; this->TX_FIFO->print();
    std::cout << "RX FIFO : "; this->RX_FIFO->print();
}

void StateMachine::execute(instr IR){
    switch((IR & instr_mask) >> 13){
            case 0b000:
                jmp(IR & ~instr_mask);
                break;
            case 0b001:
                wait(IR & ~instr_mask);
                break;
            case 0b010:
                in(IR & ~instr_mask);
                break;
            case 0b011:
                out(IR & ~instr_mask);
                break;
            case 0b100:
                //Instruction can be push, pull or invalid instr
                if(IR & 0b11111){
                    std::cerr << "Invalid instruction !  " << std::hex << IR << std::endl;
                } else {
                    if(IR & 0b10000000){
                        pull(IR & ~instr_mask);
                    } else {
                        push(IR & ~instr_mask);
                    }
                }
                break;
            case 0b101:
                mov(IR & ~instr_mask);
                break;
            case 0b110:
                irq(IR & ~instr_mask);
                break;
            case 0b111:
                set(IR & ~instr_mask);
                break;
            default:
                std::cerr << "Invalid instruction !  " << std::hex << IR << std::endl;
        }
}

void StateMachine::irq(__attribute__((unused))const uint16_t IR){
    TODO("IRQ");

    //Update PC
    this->PC ++;
}

#include "in.cpp"
#include "jmp.cpp"
#include "mov.cpp"
#include "out.cpp"
#include "pull.cpp"
#include "push.cpp"
#include "set.cpp"
#include "wait.cpp"
#ifndef _STATEMACHINE
#define _STATEMACHINE

#include <cstdint>
#include <iostream>

#include "registers.h"
#include "state_machine/instructions.h"
#include "utils.cpp"
#include "gpio.cpp"

class StateMachine {

    public:
        StateMachine(bool ISR_dir, bool OSR_dir, const unsigned int clock_divisor){
            Shift ISR = Shift(ISR_dir);
            Shift OSR = Shift(OSR_dir);
            this->ISR = &ISR;
            this->OSR = &OSR;

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

        void connect(FIFO* TX, FIFO* RX, GPIO* gpio, const instr* progmem) {
            this->TX_FIFO = TX;
            this->RX_FIFO = RX;
            this->gpio = gpio;
            this->progmem = progmem;
        }

        void run(){
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


        void conf_pin(unsigned int* pin_arr, size_t pin_arr_size){
            pin_mask = 0;
            for(unsigned int i = 0; i < pin_arr_size; i++){
                if(pin_arr[i] > 30){
                    std::cerr << "Pin " << std::to_string(pin_arr[i]) << " does not exist....";
                    return;
                }
                pin_mask = pin_mask | (1 << pin_arr[i]);
            }
        }

        void conf_in(unsigned int pin){
            this->PINCTRL_IN_BASE = pin;
        }
        void conf_jmp(unsigned int pin){
            this->EXECCTRL_JMP_PIN = pin;
        }
        void conf_out(unsigned int pin, unsigned int count){
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
        void conf_set(unsigned int pin, unsigned int count){
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
        void print(){
            std::cout << "### NEW STEP ###" << std::endl;
            std::cout << "PC : " << this->PC << std::endl;
            std::cout << "X : " << std::hex << this->X << std::endl;
            std::cout << "Y : " << std::hex << this->Y << std::endl;
        }



        private:
            // Internal Registers
            Shift *OSR, *ISR;
            uint8_t OSR_counter, ISR_counter, stall_counter;
            uint32_t X, Y, PC;
            unsigned int clock_div;
            uint32_t pin_mask;

            // External References
            FIFO* TX_FIFO, *RX_FIFO;
            const instr* progmem;
            GPIO* gpio;

            //Per instructions registers
            unsigned int PINCTRL_IN_BASE;
            unsigned int EXECCTRL_JMP_PIN;
            unsigned int PINCTRL_OUT_BASE, PINCTRL_OUT_COUNT;
            unsigned int PINCTRL_SET_BASE, PINCTRL_SET_COUNT;

            //Instructions

            void execute(instr IR){
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

            void jmp(const uint16_t IR);
            void wait(const uint16_t IR);
            void in(const uint16_t IR);
            void out(const uint16_t IR);
            void push(const uint16_t IR);
            void pull(const uint16_t IR);
            void mov(const uint16_t IR);
            void set(const uint16_t IR);

            void irq(const uint16_t IR){
                TODO("IRQ");

                //Update PC
                this->PC ++;
            }

};

#include "state_machine/in.cpp"
#include "state_machine/jmp.cpp"
#include "state_machine/mov.cpp"
#include "state_machine/out.cpp"
#include "state_machine/pull.cpp"
#include "state_machine/push.cpp"
#include "state_machine/set.cpp"
#include "state_machine/wait.cpp"

#endif
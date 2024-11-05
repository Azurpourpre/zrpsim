#pragma once

#include <cstdint>
#include "registers.h"
#include "state_machine/instructions.h"
#include "utils.cpp"
#include "gpio.h"

class StateMachine {

    public:
        StateMachine(bool ISR_dir, bool OSR_dir, const unsigned int clock_divisor);
        ~StateMachine();
        void connect(FIFO* TX, FIFO* RX, GPIO* gpio, const instr* progmem);
        void run();
        uint32_t get_pc();
        void conf_pin(unsigned int* pin_arr, size_t pin_arr_size);
        void conf_in(unsigned int pin);
        void conf_jmp(unsigned int pin);
        void conf_out(unsigned int pin, unsigned int count);
        void conf_set(unsigned int pin, unsigned int count);
        void print();

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
            void execute(instr IR);
            void jmp(const uint16_t IR);
            void wait(const uint16_t IR);
            void in(const uint16_t IR);
            void out(const uint16_t IR);
            void push(const uint16_t IR);
            void pull(const uint16_t IR);
            void mov(const uint16_t IR);
            void set(const uint16_t IR);

            void irq(const uint16_t IR);

};
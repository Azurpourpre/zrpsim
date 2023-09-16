#ifndef _STATEMACHINE
#define _STATEMACHINE

#include <cstdint>
#include <iostream>
#include "electronic.h"
#include "instructions.h"
#include "utils.cpp"

class StateMachine {

    public:
        void connect(FIFO* TX, FIFO* RX, const instr* progmem, bool ISR_dir, bool OSR_dir) {
            Shift ISR = Shift(ISR_dir);
            Shift OSR = Shift(OSR_dir);
            this->pISR = &ISR;
            this->pOSR = &OSR;

            this->TX_FIFO = TX;
            this->RX_FIFO = RX;
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
                instr ISR = this->progmem[this->PC];

                //Decode & Execute
                switch((ISR & instr_mask) >> 13){
                    case 0b000:
                        jmp(ISR & ~instr_mask);
                        break;
                    case 0b001:
                        wait(ISR & ~instr_mask);
                        break;
                    case 0b010:
                        TODO("IN");
                        break;
                    case 0b011:
                        TODO("OUT");
                        break;
                    case 0b100:
                        //Instruction can be push, pull or invalid instr
                        if(ISR & 0b11111){
                            std::cerr << "Invalid instruction !  " << std::hex << ISR << std::endl;
                        } else {
                            if(ISR & 0b10000000){
                                TODO("PULL");
                            } else {
                                TODO("PUSH");
                            }
                        }
                        break;
                    case 0b101:
                        TODO("MOV");
                        break;
                    case 0b110:
                        TODO("IRQ");
                        break;
                    case 0b111:
                        TODO("SET");
                        break;
                    default:
                        std::cerr << "Invalid instruction !  " << std::hex << ISR << std::endl;
                }

            }
        }

        // Debug
        void print(){

            std::cout << "PC : " << this->PC << std::endl;
            std::cout << "X : " << std::hex << this->X << std::endl;
            std::cout << "Y : " << std::hex << this->Y << std::endl;
        }

        private:
        // Internal Registers
        Shift *pOSR, *pISR;
        uint8_t OSR_counter, ISR_counter, stall_counter;
        uint32_t X, Y, PC;

        // External References
        FIFO* TX_FIFO, *RX_FIFO;
        const instr* progmem;

        //Instructions

        void jmp(const uint16_t ISR){
            //Set the stall
            this->stall_counter = (ISR & delay_sideset_mask) >> 8;

            //Check condition
            bool verified;
            switch ((ISR & 0b11100000) >> 5){
                case 0b000:
                    verified = true;
                    break;
                case 0b001:
                    verified = (this->X == 0);
                    break;
                case 0b010:
                    verified = (this->X != 0);
                    this->X --;
                    break;
                case 0b011:
                    verified = (this->Y == 0);
                    break;
                case 0b100:
                    verified = (this->Y != 0);
                    this->Y --;
                    break;
                case 0b101:
                    verified = (X!=Y);
                    break;
                case 0b110:
                    TODO("PIN");
                    break;
                case 0b111:
                    TODO("SHIFT REGISTER COUNTER");
                    break;
                default:
                    std::cerr << "Condition not known !" << std::endl;
            }

            if (verified) {
                this->PC = ISR & 0b11111;
            }
        }

        void wait(const uint16_t ISR){
            //Check condition
            bool pol = ISR & (1 << 7);
            uint8_t index = ISR & 0b11111;
            bool verified;

            switch ((ISR & (0b11 << 5)) >> 5) {
                case 0b00:
                    TODO("GPIO");
                    break;
                case 0b01:
                    TODO("GPIO");
                    break;
                case 0b10:
                    TODO("IRQ");
                    break;
                default:
                    std::cerr << "Invalid WAIT condition" << std::endl;
            }

            if (verified) {
                this->PC ++;
                this->stall_counter = (ISR & delay_sideset_mask) >> 8;
            } 
            // Else we dont do anything bc we want to check at next cycle again
        }

        void in(const uint16_t ISR){
            //Set the stall
            this->stall_counter = (ISR & delay_sideset_mask) >> 8;

            //Get bitcount and source
            uint8_t bitcount = ISR & 0b11111;
            bitcount = (bitcount == 0) ? 32 : bitcount;

            switch ((ISR & 0b11100000) >> 5) {
                case 0b000:
                    TODO("GPIO");
                    break;
                case 0b001:
                    this->pISR->shift(this->X, bitcount);
                    break;
                case 0b010:
                    this->pISR->shift(this->Y, bitcount);
                    break;
                case 0b011:
                    this->pISR->shift(0, bitcount);
                    break;
                case 0b100:
                case 0b101:
                    std::cerr << "Reserved value in IN" << std::endl;
                    break;
                case 0b110:
                    (*this->pISR).shift(
                        (*this->pISR).get(),
                        bitcount);
                    break;
                case 0b111:
                    (*this->pISR).shift(
                        (*this->pOSR).get(),
                        bitcount);
                    break;
            }

            // Update ISR count
            ISR_counter += bitcount;
            ISR_counter = (ISR_counter > 32) ? 32 : ISR_counter;

            //Update PC
            this->PC ++;
        }

        void out(const uint16_t ISR){
            //Set the stall
            this->stall_counter = (ISR & delay_sideset_mask) >> 8;

            //Get bitcount and destination
            uint8_t bitcount = ISR & 0b11111;
            bitcount = (bitcount == 0) ? 32 : bitcount;

            switch ((ISR & 0b11100000) >> 5) {
                case 0b000:
                    TODO("GPIO");
                    break;
                case 0b001:
                    this->X = this->pOSR->shift(0, bitcount);
                    break;
                case 0b010:
                    this->Y = this->pOSR->shift(0, bitcount);
                    break;
                case 0b011:
                    this->pOSR->shift(0, bitcount);
                    break;
                case 0b100:
                    TODO("GPIO");
                    break;
                case 0b101:
                    this->PC = this->pOSR->shift(0, bitcount);
                    return;
                case 0b110:
                    this->pISR->shift(
                        this->pOSR->shift(0, bitcount),
                        bitcount
                    );
                    break;
                case 0b111:
                    TODO("OUT (j'ai rien compris)");
                    break;
            }

            // Update PC
            this->PC ++;
        }
    

};

#endif
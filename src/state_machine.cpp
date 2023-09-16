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
            this->ISR = &ISR;
            this->OSR = &OSR;

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
                instr IR = this->progmem[this->PC];

                //Decode & Execute
                this->execute(IR);

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
        Shift *OSR, *ISR;
        uint8_t OSR_counter, ISR_counter, stall_counter;
        uint32_t X, Y, PC;

        // External References
        FIFO* TX_FIFO, *RX_FIFO;
        const instr* progmem;

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
                        TODO("MOV");
                        break;
                    case 0b110:
                        TODO("IRQ");
                        break;
                    case 0b111:
                        TODO("SET");
                        break;
                    default:
                        std::cerr << "Invalid instruction !  " << std::hex << IR << std::endl;
                }
        }

        void jmp(const uint16_t IR){
            //Set the stall
            this->stall_counter = (IR & delay_sideset_mask) >> 8;

            //Check condition
            bool verified;
            switch ((IR & 0b11100000) >> 5){
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
                this->PC = IR & 0b11111;
            }
        }

        void wait(const uint16_t IR){
            //Check condition
            bool pol = IR & (1 << 7);
            uint8_t index = IR & 0b11111;
            bool verified;

            switch ((IR & (0b11 << 5)) >> 5) {
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
                this->stall_counter = (IR & delay_sideset_mask) >> 8;
            } 
            // Else we dont do anything bc we want to check at next cycle again
        }

        void in(const uint16_t IR){
            //Set the stall
            this->stall_counter = (IR & delay_sideset_mask) >> 8;

            //Get bitcount and source
            uint8_t bitcount = IR & 0b11111;
            bitcount = (bitcount == 0) ? 32 : bitcount;

            switch ((IR & 0b11100000) >> 5) {
                case 0b000:
                    TODO("GPIO");
                    break;
                case 0b001:
                    this->ISR->shift(this->X, bitcount);
                    break;
                case 0b010:
                    this->ISR->shift(this->Y, bitcount);
                    break;
                case 0b011:
                    this->ISR->shift(0, bitcount);
                    break;
                case 0b100:
                case 0b101:
                    std::cerr << "Reserved value in IN" << std::endl;
                    break;
                case 0b110:
                    (*this->ISR).shift(
                        (*this->ISR).get(),
                        bitcount);
                    break;
                case 0b111:
                    (*this->ISR).shift(
                        (*this->OSR).get(),
                        bitcount);
                    break;
            }

            // Update ISR count
            ISR_counter += bitcount;
            ISR_counter = (ISR_counter > 32) ? 32 : ISR_counter;

            //Update PC
            this->PC ++;
        }

        void out(const uint16_t IR){
            //Set the stall
            this->stall_counter = (IR & delay_sideset_mask) >> 8;

            //Get bitcount and destination
            uint8_t bitcount = IR & 0b11111;
            bitcount = (bitcount == 0) ? 32 : bitcount;

            switch ((IR & 0b11100000) >> 5) {
                case 0b000:
                    TODO("GPIO");
                    break;
                case 0b001:
                    this->X = this->OSR->shift(0, bitcount);
                    break;
                case 0b010:
                    this->Y = this->OSR->shift(0, bitcount);
                    break;
                case 0b011:
                    this->OSR->shift(0, bitcount);
                    break;
                case 0b100:
                    TODO("GPIO");
                    break;
                case 0b101:
                    this->PC = this->OSR->shift(0, bitcount);
                    return;
                case 0b110:
                    this->ISR->shift(
                        this->OSR->shift(0, bitcount),
                        bitcount
                    );
                    break;
                case 0b111:
                    this->execute(this->OSR->shift(0, bitcount));
                    return;
            }

            // Update PC
            this->PC ++;
        }

        void push(const uint16_t IR){
            //Set the stall
            this->stall_counter = (IR & delay_sideset_mask) >> 8;

            if ( (IR & 0b1000000) && (this->ISR_counter >= 32) ){
                // If the isFull bit is set and ISR is below threshold
                TODO("AutoPush");
                return;

            } else if ( (IR & 0b100000) && (this->RX_FIFO->get_size() <= 3*32) ){
                // Block bit is set and TX FIFO is empty
                // So stall
                return;

            } else {
                // Everything is fine, so push ISR into RX FIFO
                this->RX_FIFO->push(
                    this->ISR->get(),
                    32);

                // And clear out ISR
                this->ISR->fill(0);
                this->ISR_counter = 0;

                this->PC ++;
            }

        }

        void pull(const uint16_t IR){
            //Set the stall
            this->stall_counter = (IR & delay_sideset_mask) >> 8;

            if ( (IR & 0b1000000) && (this->ISR_counter <= 0) ){
                // If the isEmpty bit is set and OSR is below threshold
                TODO("AutoPush");
                return;

            } else if (this->TX_FIFO->get_size() == 0) {
                //If TX FIFO is empty
                //Look the block bit
                if (IR & 0b100000) {
                    //If the block bit is set, we stall
                    return;
                } else {
                    // If the block bit is not set, we copy X to OSR
                    this->OSR->fill(X);
                    this->OSR_counter = 32;
                    this->PC ++;
                }
            } else {
                // TX fifo is not empty
                // So we pull the TX FIFO into OSR
                this->OSR->fill(
                    this->TX_FIFO->pull(32)
                );
                this->OSR_counter = 32;
                this->PC ++;
            }
        }
    

};

#endif
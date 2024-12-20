#include "shift.h"
Shift::Shift(const bool direction){
    // The only option is the shift direction
    // We define :
    //    0 -> left shift
    //    1 -> right shift

    this->direction = direction;
    this->buffer = 0;
}

void Shift::fill(const uint32_t input){
    // Fill buffer with the input
    this->buffer = input;
}

uint32_t Shift::get(){
    return this->buffer;
}

bool Shift::shift(const bool input){
    // Shift the register according to self.direction
    // Input is the new bit being shifted in
    // Output is the overflow bit

    bool res;

    if(this->direction){
        // Right shift
        res = this->buffer & 0b1;
        this->buffer = (this->buffer >> 1) + (input ? 1 << 31 : 0);
    }
    else {
        // Left shift
        res = this->buffer & (uint32_t)(1<<31);
        this->buffer = (this->buffer << 1) + (input ? 1 : 0);
    }

    return res;
}

uint32_t Shift::shift(const uint32_t input, uint8_t bitcount){
    // Shift the register according to self.direction
    // Input is the new bit being shifted in
    // Output is the overflow bit
    uint32_t res;
    uint32_t mask = 0;

    if(this->direction){
        // Right shift

        for(int i = 0; i < bitcount; i++){
            mask = (mask << 1) + 1;
        }

        res = this->buffer & mask;
        this->buffer = (this->buffer >> bitcount) + ((input & mask) << bitcount);
    }
    else {
        // Left shift

        for(int i = 0; i < bitcount; i++) {
            mask = (mask >> 1) + (1 << 31);
        }

        res = this->buffer & mask;
        this->buffer = (this->buffer << bitcount) + (input & mask);
    }

    return res;
}
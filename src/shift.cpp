#ifndef _SHIFTER
#define _SHIFTER

#include <cstdint>

class Shift{
    // 32 bit shift register

    private:
        uint32_t buffer;
        bool direction;
    

    public:
        Shift(bool direction){
            // The only option is the shift direction
            // We define :
            //    0 -> left shift
            //    1 -> right shift

            this->direction = direction;
        }

        void fill(uint32_t input){
            // Fill buffer with the input
            this->buffer = input;
        }

        uint32_t get(){
            return this->buffer;
        }

        bool shift(bool input){
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
                res = this->buffer & (const uint32_t)(1<<31);
                this->buffer = (this->buffer << 1) + (input ? 1 : 0);
            }

            return res;
        }
};

#endif
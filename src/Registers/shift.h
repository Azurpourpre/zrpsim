#pragma once

#include <cstdint>

class Shift{
    // 32 bit shift register
    public:
        Shift(const bool direction);
        void fill(const uint32_t input);
        uint32_t get();
        bool shift(const bool input);
        uint32_t shift(const uint32_t input, uint8_t bitcount);

    private:
        uint32_t buffer;
        bool direction;
};
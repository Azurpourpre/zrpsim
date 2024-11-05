#pragma once
#include <cstdint>
#include <iostream>


class FIFO {
    public:
        FIFO();
        void push(const uint32_t value, const uint8_t size);
        uint32_t pull(const uint8_t size);
        uint8_t get_size();
        void print();

    private:
        uint32_t buffer[4];
        uint8_t size;
};
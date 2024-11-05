#include "fifo.h"

FIFO::FIFO() {
    this->size = 0;
    this->buffer[0] = 0;
    this->buffer[1] = 0;
    this->buffer[2] = 0;
    this->buffer[3] = 0;
}

void FIFO::push(const uint32_t value, const uint8_t size){
    if (size > 32) {
        std::cerr << "Can't push more than 32 bits in the FIFO at once" << std::endl;
    }

    // mask is a sequence of 1, of len size
    uint32_t mask;
    if (size < 32) {
        mask = (1 << size) - 1;
    } else {
        mask = 0xffffffff;
    }

    this->buffer[3] = ((this->buffer[3] & ~mask) >> size) | ((this->buffer[2] & mask) << (32 - size));
    this->buffer[2] = ((this->buffer[2] & ~mask) >> size) | ((this->buffer[1] & mask) << (32 - size));
    this->buffer[1] = ((this->buffer[1] & ~mask) >> size) | ((this->buffer[0] & mask) << (32 - size));
    this->buffer[0] = ((this->buffer[0] & ~mask) >> size) | ((value & mask) << (32 - size));

    this->size += size;
}

uint32_t FIFO::pull(const uint8_t size){
    if (size > 32) {
        std::cerr << "Can't pull more than 32 bits from the FIFO at once" << std::endl;
    }

    // mask is a sequence of 1, of len size
    uint32_t mask = (1 << size) - 1;

    const uint32_t rep = this->buffer[3] & mask;
    uint8_t old_size = this->size;

    this->push(0, size);

    this->size = old_size - size;

    return rep;
}

uint8_t FIFO::get_size(){
    return this->size;
}

void FIFO::print(){
    std::cout
        << "0x" << std::hex << this->buffer[0] << " "
        << "0x" << std::hex << this->buffer[1] << " "
        << "0x" << std::hex << this->buffer[2] << " "
        << "0x" << std::hex << this->buffer[3] << " " << std::endl;
}
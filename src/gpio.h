#pragma once

#include <fstream>
#include <cstdint>
#include <vector>
#include <map>
#include <unordered_map>
#include "registers.h"

typedef struct VCDSignal_t {
    uint8_t size;
    uint32_t mask;
    const char identifider;
    const char* name;
} VCDSignal;

class CustomReader_GPIO{
    public:
        CustomReader_GPIO(const char* filename);
        float get_dt();
        uint32_t update_gpio_state(uint32_t pinmap, float time);
        void update_fifo_state(FIFO* fifo, float time);

    private:
        std::ifstream infile;
        float dt = 1;
        std::map<int, std::map<int, uint32_t>> playback_map = {};
        int signal_id[128]; //Char to int
    
};

class VCDWriter{
    public:
        VCDWriter(const char* filename);
        ~VCDWriter();
        void write_header(const char* timescale);
        void write_time(const char* timestamp);
        void write_value(VCDSignal* sig, const uint32_t val);
        VCDSignal* create_signal(const uint8_t size, std::string wanted_name);

    private:
        std::ofstream outfile;
        char n_signal;
        std::unordered_map<VCDSignal*, uint32_t> state; // Vector of all signals
};

class GPIO {
    public:
        GPIO(VCDWriter* writer, CustomReader_GPIO* reader, const float freq);
        ~GPIO();
        void write_pin(unsigned int pin, const bool state);
        void next_time();
        float get_time();
        uint32_t get_pinState();
        bool get_pin(uint8_t pinNumber);

    private:
        VCDWriter* writer;
        CustomReader_GPIO* reader;
        unsigned int t;
        float dt;
        uint32_t pinState;
        uint32_t old_pinState;
        VCDSignal* siglist[30];
};
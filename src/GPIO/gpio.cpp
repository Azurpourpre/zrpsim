#include "../gpio.h"
#include "../utils.cpp"
#include "../registers.h"
#include <iomanip>
#include <cstring>
#include <iostream>
#include <string>
#include <regex>
#include <cmath>
#include "vcdwriter.cpp"
#include "input.cpp"

std::string freq_to_ts(const float freq){
    std::string rep;
    if(freq < 1){
        // freq < 1Hz then rep is in seconds
        rep = std::to_string((unsigned int)(1/freq)) + "s";
    }
    else if(freq < 10e3){
        // 1Hz < freq < 1kHz then rep is in ms
        rep = std::to_string((unsigned int)(10e3/freq)) + "ms";
    }
    else if(freq < 10e6){
        // 1kHz < freq < 1MHz then rep is in us
        rep = std::to_string((unsigned int)(10e6/freq)) + "us";
    }
    else if(freq < 10e9){
        // 1MHz < freq < 1 GHz then rep is in ns
        rep = std::to_string((unsigned int)(10e9/freq)) + "ns";
    }
    else if(freq < 10e12){
        // 1GHz < freq < 1THz then rep is in ps
        rep = std::to_string((unsigned int)(10e12/freq)) + "ps";
    }
    else if(freq < 10e15){
        // 1THz < freq < 1PHz then rep is in ns
        rep = std::to_string((unsigned int)(10e15/freq)) + "fs";
    }
    else {
        std::cerr << "GPIO cant go faster than PHz sorry (wtf bro).\nFallback on 1s per step." << std::endl;
        rep = std::string("1s");
    }

    return rep;
}

GPIO::GPIO(VCDWriter* writer, CustomReader_GPIO* reader, const float freq){
    this->writer = writer;
    this->reader = reader;
    this->pinState = 0;
    this->old_pinState = 0;
    this->dt = 1/freq;
    this->t = 1;

    if(this->writer != NULL){
        for(int i = 0; i < 30; i++){
            const std::string name = "Pin_" + std::to_string(i);
            this->siglist[i] = this->writer->create_signal(1, name);
        }
        this->writer->write_header(freq_to_ts(freq).c_str());
    }
}

GPIO::~GPIO(){
    if(this->writer != NULL){
        for(int i = 0; i < 30; i++){
            delete[] this->siglist[i]->name;
            delete this->siglist[i];
        }
    }
}

void GPIO::write_pin(unsigned int pin, const bool state){
    if(state > 0){
        pinState = pinState | (1 << pin);
    }
    else {
        pinState = pinState & ~(1 << pin);
    }
}

void GPIO::next_time(){
    uint32_t diff = pinState ^ old_pinState;
    unsigned int pin = 0;

    if(this->writer != NULL && diff != 0){
        this->write_time();

        while((diff != 0) && (pin < 30)){
            // We have a changed bit !
            if(diff & 0b1){
                //The bit changed is the current pin
                this->writer->write_value(
                    this->siglist[pin],
                    (pinState >> pin) & 0b1
                );
            }

            pin++;
            diff = diff >> 1;
        }
    }

    this->old_pinState = this->pinState;
    if(reader != NULL)
        this->pinState = this->reader->update_gpio_state(this->pinState, this->get_time());

    this->t++;
}

void GPIO::write_time(){
    this->writer->write_time(std::to_string(this->t).c_str());
}

float GPIO::get_time(){
    return this->t*this->dt;
}

uint32_t GPIO::get_pinState(){
    return this->pinState;
}

bool GPIO::get_pin(uint8_t pinNumber){
    if(pinNumber <= 30){
        return this->pinState & (1 << pinNumber);
    }
    else{
        std::cerr << "Tried to query state of invalid pin" << std::endl;
        return false;
    }
}
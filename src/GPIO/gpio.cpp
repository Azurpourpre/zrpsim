#ifndef _GPIO
#define _GPIO

#include <cstdint>
#include <string>
#include <cmath>
#include <vector>
#include "vcd.cpp"
#include "../utils.cpp"

class GPIO {
    public:
        GPIO(VCDWriter* writer, const float freq){
            this->writer = writer;
            for(int i = 0; i < 30; i++){
                const std::string name = "Pin_" + std::to_string(i);
                this->siglist[i] = writer->create_signal(1, name.c_str());
            }

            this->pinState = 0;
            this->old_pinState = 0;

            writer->write_header(this->freq_to_ts(freq));

            this->t = 1;


        }

        void write_pin(unsigned int pin, const bool state){
            if(state > 0){
                pinState = pinState | (1 << pin);
            }
            else {
                pinState = pinState & ~(1 << pin);
            }
        }

        void next_time(){
            uint32_t diff = pinState ^ old_pinState;
            unsigned int pin = 0;

            this->writer->write_time(std::to_string(t).c_str());
            t++;

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
            this->old_pinState = this->pinState;
        }
    private:
        VCDWriter* writer;
        unsigned int t;
        uint32_t pinState;
        uint32_t old_pinState;
        VCDSignal* siglist[30];

        const char* freq_to_ts(const float freq){
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

            return rep.c_str();
        }
};

#endif
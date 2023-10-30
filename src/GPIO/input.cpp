#ifndef _INPUT
#define _INPUT
//We use a custom input file format here

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <regex>

#define FIFO_ID 31

class CustomReader_GPIO{
    private:
        std::ifstream infile;
        float dt = 1;
        std::map<int, std::map<int, uint32_t>> playback_map = {};
        int signal_id[128]; //Char to int
    
    public:
        CustomReader_GPIO(const char* filename){
            
            //Open file
            this->infile.open(filename, std::ios::in);

            //Read Header
            std::string line;
            std::regex  re_timescale("\\$timescale [0-9]+(s|ms|us|ns|ps|fs) \\$end"),
                        re_var("\\$var wire [0-9]+ . \\w+ \\$end"),
                        re_endheader("\\$enddefinitions \\$end");

            while(getline(this->infile, line)){
                if(std::regex_match(line, re_timescale)){
                    // Timescale token
                    // So we affect the value to dt
                    const std::regex re_unit("(s|ms|us|ns|ps|fs)"), re_num("[0-9]+");

                    std::smatch match;
                    std::regex_search(line, match, re_num);
                    const int number = std::stoi(match.str());
                    std::regex_search(line, match, re_unit);
                    const std::string unit = match.str();

                    if(unit == "s"){
                        this->dt = number;
                    }
                    else if(unit == "ms"){
                        this->dt = number * 1e-3;
                    }
                    else if(unit == "us"){
                        this->dt = number * 1e-6;
                    }
                    else if(unit == "ns"){
                        this->dt = number * 1e-9;
                    }
                    else if(unit == "ps"){
                        this->dt = number * 1e-12;
                    }
                    else if(unit == "fs"){
                        this->dt = number * 1e-15;
                    }
                    else{
                        std::cerr << "Invalid Unit !" << std::endl;
                    }
                }
                else if(std::regex_match(line, re_var)){
                    // Var token
                    // So we add the data to the playback list.
                    std::regex re_FIFO("\\$var wire 32 . FIFO \\$end"), re_Pin("\\$var wire 1 . pin_\\d{1,2} \\$end");
                    if(std::regex_match(line, re_Pin)){
                        char id = line.c_str()[12];
                        int pin_num = std::stoi(line.substr(18, 2));

                        //Sanitiy Check
                        if(pin_num > 30){
                            std::cerr << "Invalid Pin Number" << std::endl;
                        }
                        else if(id < 33 || id > 127 || id == '0' || id == '1') {
                            std::cerr << "Invalid Signal Identifier" << std::endl;
                        } else {
                            this->signal_id[id] = pin_num;
                        }
                    }
                    else if(std::regex_match(line, re_FIFO)){
                        char id = line.c_str()[13];
                        this->signal_id[id] = FIFO_ID;
                    }
                }
                else if(std::regex_match(line, re_endheader)){
                    break;
                }
            }

            //Read content
            int time;
            std::regex re_time("#\\d+"), re_val("([0-9]+|0x[0-f]+).");
            
            while(getline(this->infile, line)){
                if(std::regex_match(line, re_time)){
                    time = std::stoi(line.substr(1));
                 playback_map[time] = {};
                }
                if(std::regex_match(line, re_val)){
                    const int value = std::stoi(line, nullptr, 0);
                    const int pin = signal_id[line.back()];
                    if(pin <= 30){
                        this->playback_map[time][pin] = value & 0b1;
                    }
                    else if(pin == FIFO_ID){
                        this->playback_map[time][pin] = value & 0xffffffff;
                    }
                }
            }

            //Close file
            this->infile.close();
        }

        float get_dt(){ return this->dt; }

        uint32_t update_gpio_state(uint32_t pinmap, float time){
            const int timestep = round(time / dt);
            uint32_t retval = pinmap;

            if(this->playback_map.find(timestep) != this->playback_map.end()){
                //If we have an update for this timestamp
                std::map<int, uint32_t> actions = this->playback_map[timestep];
                for(const auto& [pin, val]: actions){
                    if(pin < 31){
                        //GPIO pin
                        retval = (val == 1) ? retval | (1 << pin) : retval & ~(1 << pin);
                    }
                }
            }

            return retval;
        }

        void update_fifo_state(FIFO* fifo, float time){
            const int timestep = round(time / dt);

            if(this->playback_map.find(timestep) != this->playback_map.end()){
                //If we have an update for this timestamp
                fifo->push(this->playback_map[timestep][FIFO_ID], 32);
            }
        }
};

#endif
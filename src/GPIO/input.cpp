//We use a custom input file format here

#define FIFO_ID 31

CustomReader_GPIO::CustomReader_GPIO(const char* filename){
    
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
            const std::regex re_val("[0-9]+(m|n|p|f)?s");

            std::smatch match;
            std::regex_search(line, match, re_val);

            int number = 0;
            std::string unit = "";
            
            for(char& c : match.str()){
                if(('0' <= c) && (c <= '9'))
                    number = (number * 10) + (c - '0');
                else 
                    unit += c;
            }

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
                char id = line[12];
                int pin_num = std::stoi(line.substr(18, 2));

                //Sanitiy Check
                if(pin_num > 30){
                    std::cerr << "Invalid Pin Number" << std::endl;
                }
                else if(id < 33 || id == '0' || id == '1') {
                    std::cerr << "Invalid Signal Identifier" << std::endl;
                } else {
                    this->signal_id[(unsigned int)id] = pin_num;
                }
            }
            else if(std::regex_match(line, re_FIFO)){
                char id = line.c_str()[13];
                this->signal_id[(unsigned int)id] = FIFO_ID;
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
        else if(std::regex_match(line, re_val)){
            const unsigned long value = std::stoul(line, nullptr, 0);
            const int pin = signal_id[(unsigned int)line.back()];
            if(pin <= 30){
                this->playback_map[time][pin] = value & 0b1;
            }
            else if(pin == FIFO_ID){
                this->playback_map[time][pin] = value & 0xffffffff;
            }
        }
    }
    
    this->max_time = (time + 1)*this->dt;

    //Close file
    this->infile.close();
}

float CustomReader_GPIO::get_dt(){ return this->dt; }

uint32_t CustomReader_GPIO::update_gpio_state(uint32_t pinmap, float time){
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

void CustomReader_GPIO::update_fifo_state(FIFO* fifo, float time){
    const int timestep = round(time / dt);

    if(this->playback_map.find(timestep) != this->playback_map.end() && this->playback_map.at(timestep).find(FIFO_ID) != this->playback_map.at(timestep).end()){
        fifo->push(this->playback_map.at(timestep).at(FIFO_ID), 32);
    }
}

float CustomReader_GPIO::get_max_time(){
    return this->max_time;
}
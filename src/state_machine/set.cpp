void StateMachine::set(const uint16_t IR){
    //Set the stall
    this->stall_counter = (IR & delay_sideset_mask) >> 8;

    uint8_t data = IR & 0b11111;

    unsigned int pin = this->PINCTRL_SET_BASE;
    switch( (IR & 0b11100000) >> 5){
        case 0b000:
        case 0b100:
            for(unsigned int i = 0; i < this->PINCTRL_SET_COUNT; i++){
                this->gpio->write_pin(pin, data);
                data = data >> 1;
                pin = (pin + 1) % 31;
            }
            break;
        case 0b001:
            this->X = data;
            break;
        case 0b010:
            this->Y = data;
            break;
        case 0b011:
        case 0b101:
        case 0b110:
        case 0b111:
            std::cerr << "Reserved destination in SET" << std::endl;
            break;
    }

    //Update PC
    this->PC ++;
}
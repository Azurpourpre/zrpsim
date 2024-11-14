void StateMachine::in(const uint16_t IR){
    //Set the stall
    this->stall_counter = (IR & delay_sideset_mask) >> 8;

    //Get bitcount and source
    uint8_t bitcount = IR & 0b11111;
    bitcount = (bitcount == 0) ? 32 : bitcount;

    switch ((IR & 0b11100000) >> 5) {
        case 0b000:
            this->ISR->shift(std::rotr(this->gpio->get_pinState(), this->PINCTRL_IN_BASE + 1), bitcount);
            break;
        case 0b001:
            this->ISR->shift(this->X, bitcount);
            break;
        case 0b010:
            this->ISR->shift(this->Y, bitcount);
            break;
        case 0b011:
            this->ISR->shift(0, bitcount);
            break;
        case 0b100:
        case 0b101:
            std::cerr << "Reserved value in IN" << std::endl;
            break;
        case 0b110:
            (*this->ISR).shift(
                (*this->ISR).get(),
                bitcount);
            break;
        case 0b111:
            (*this->ISR).shift(
                (*this->OSR).get(),
                bitcount);
            break;
    }

    // Update ISR count
    ISR_counter += bitcount;
    ISR_counter = (ISR_counter > 32) ? 32 : ISR_counter;

    //Update PC
    this->PC ++;
}
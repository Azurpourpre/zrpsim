void StateMachine::out(const uint16_t IR){
    //Set the stall
    this->stall_counter = (IR & delay_sideset_mask) >> 8;

    //Get bitcount and destination
    uint8_t bitcount = IR & 0b11111;
    bitcount = (bitcount == 0b00000) ? 32 : bitcount;

    uint32_t pinout;
    unsigned int pin;
    switch ((IR & 0b11100000) >> 5) {
        case 0b000:
        case 0b100:
            pinout = this->OSR->shift(0, bitcount);
            pin = this->PINCTRL_OUT_BASE;
            for(int i = 0; i < this->PINCTRL_OUT_COUNT; i++){
                this->gpio->write_pin(pin, pinout & 0b1);
                pinout = pinout >> 1;
                pin = (pin + 1)%31;
            }
            break;
        case 0b001:
            this->X = this->OSR->shift(0, bitcount);
            break;
        case 0b010:
            this->Y = this->OSR->shift(0, bitcount);
            break;
        case 0b011:
            this->OSR->shift(0, bitcount);
            break;
        case 0b101:
            this->PC = this->OSR->shift(0, bitcount);
            return;
        case 0b110:
            this->ISR->shift(
                this->OSR->shift(0, bitcount),
                bitcount
            );
            break;
        case 0b111:
            this->execute(this->OSR->shift(0, bitcount));
            return;
    }

    OSR_counter += bitcount;
    OSR_counter = (OSR_counter > 32) ? 32 : OSR_counter;

    // Update PC
    this->PC ++;
}
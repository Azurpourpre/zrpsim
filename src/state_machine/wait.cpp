void StateMachine::wait(const uint16_t IR){
    //Check condition
    bool pol = IR & (1 << 7);
    uint8_t index = IR & 0b11111;
    bool verified = false;

    switch ((IR & (0b11 << 5)) >> 5) {
        case 0b00:
            verified = (this->gpio->get_pin(index) == pol);
            break;
        case 0b01:
            verified = (this->gpio->get_pin((index + this->PINCTRL_IN_BASE)%32) == pol);
            break;
        case 0b10:
            TODO("IRQ");
            break;
        default:
            std::cerr << "Invalid WAIT condition" << std::endl;
    }

    if (verified) {
        this->PC ++;
        this->stall_counter = (IR & delay_sideset_mask) >> 8;
    } 
    // Else we dont do anything bc we want to check at next cycle again
}
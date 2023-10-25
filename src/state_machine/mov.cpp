uint32_t reverseBits(uint32_t n) {
    n = (n >> 1) & 0x55555555 | (n << 1) & 0xaaaaaaaa;
    n = (n >> 2) & 0x33333333 | (n << 2) & 0xcccccccc;
    n = (n >> 4) & 0x0f0f0f0f | (n << 4) & 0xf0f0f0f0;
    n = (n >> 8) & 0x00ff00ff | (n << 8) & 0xff00ff00;
    n = (n >> 16) & 0x0000ffff | (n << 16) & 0xffff0000;
    return n;
}

void StateMachine::mov(const uint16_t IR){
    //Set the stall
    this->stall_counter = (IR & delay_sideset_mask) >> 8;

    //Get the source
    uint32_t source_value;
    switch (IR & 0b111){
        case 0b000:
            TODO("INPUT GPIO");
            break;
        case 0b001:
            source_value = this->X;
            break;
        case 0b010:
            source_value = this->Y;
            break;
        case 0b011:
            source_value = 0;
            break;
        case 0b100:
            std::cerr << "Reserved source used in MOV" << std::endl;
            break;
        case 0b101:
            TODO("STATUS");
            break;
        case 0b110:
            source_value = this->ISR->get();
            break;
        case 0b111:
            source_value = this->OSR->get();
            break;
    }

    //Apply Operation
    switch( (IR & 0b11000) >> 3){
        case 0b00:
            //Do nothing
            break;
        case 0b01:
            source_value = ~source_value;
            break;
        case 0b10:
            source_value = reverseBits(source_value);
            break;
        case 0b11:
            std::cerr << "Reserved operation used in mov" << std::endl;
            break;
    }

    //Send to destination
    switch( (IR & 0b11100000) >> 5 ){
        case 0b000:
            for(int i = 0; i < this->PINCTRL_OUT_COUNT; i++){
                this->gpio->write_pin((this->PINCTRL_OUT_BASE + i)%31, source_value & 0b1);
                source_value = source_value >> 1;
            }
            break;
        case 0b001:
            this->X = source_value;
            break;
        case 0b010:
            this->Y = source_value;
            break;
        case 0b011:
            std::cerr << "Reserved destination in MOV" << std::endl;
            break;
        case 0b100:
            this->execute(source_value);
            break;
        case 0b101:
            this->PC = source_value;
            break;
        case 0b110:
            this->ISR->fill(source_value);
            this->ISR_counter = 0;
            break;
        case 0b111:
            this->OSR->fill(source_value);
            this->OSR_counter = 32;
            break;
    }

    //Update PC
    this->PC ++;
}
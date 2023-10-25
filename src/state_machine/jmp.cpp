void StateMachine::jmp(const uint16_t IR){
    //Set the stall
    this->stall_counter = (IR & delay_sideset_mask) >> 8;

    //Check condition
    bool verified;
    switch ((IR & 0b11100000) >> 5){
        case 0b000:
            verified = true;
            break;
        case 0b001:
            verified = (this->X == 0);
            break;
        case 0b010:
            verified = (this->X != 0);
            if(verified)
                this->X --;
            break;
        case 0b011:
            verified = (this->Y == 0);
            break;
        case 0b100:
            verified = (this->Y != 0);
            this->Y --;
            break;
        case 0b101:
            verified = (X!=Y);
            break;
        case 0b110:
            TODO("INPUT GPIO");
            break;
        case 0b111:
            TODO("SHIFT REGISTER COUNTER");
            break;
        default:
            std::cerr << "Condition not known !" << std::endl;
    }

    if (verified) {
        this->PC = IR & 0b11111;
    }
}
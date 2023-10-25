void StateMachine::pull(const uint16_t IR){
    //Set the stall
    this->stall_counter = (IR & delay_sideset_mask) >> 8;

    if ( (IR & 0b1000000) && (this->ISR_counter <= 0) ){
        // If the isEmpty bit is set and OSR is below threshold
        TODO("AutoPush");
        return;

    } else if (this->TX_FIFO->get_size() == 0) {
        //If TX FIFO is empty
        //Look the block bit
        if (IR & 0b100000) {
            //If the block bit is set, we stall
            return;
        } else {
            // If the block bit is not set, we copy X to OSR
            this->OSR->fill(X);
            this->OSR_counter = 32;
            this->PC ++;
        }
    } else {
        // TX fifo is not empty
        // So we pull the TX FIFO into OSR
        this->OSR->fill(
            this->TX_FIFO->pull(32)
        );
        this->OSR_counter = 32;
        this->PC ++;
    }
}
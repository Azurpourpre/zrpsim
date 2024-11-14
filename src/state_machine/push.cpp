void StateMachine::push(const uint16_t IR){
    //Set the stall
    this->stall_counter = (IR & delay_sideset_mask) >> 8;

    if ( (IR & 0b1000000) && (this->ISR_counter >= 32) ){
        // If the isFull bit is set and ISR is above threshold
        TODO("AutoPush");
        return;

    } else if ( (IR & 0b100000) && (this->RX_FIFO->get_size() == 4*32) ){
        // Block bit is set and RX_FIFO is full
        // So stall
        return;

    } else {
        // Everything is fine, so push ISR into RX FIFO
        this->RX_FIFO->push(
            this->ISR->get(),
            32);

        // And clear out ISR
        this->ISR->fill(0);
        this->ISR_counter = 0;

        this->PC ++;
    }

}
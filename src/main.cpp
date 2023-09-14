#include <iostream>

#include "fifo.cpp"


int main(){
    FIFO f = FIFO();

    f.push(0xff, 7);
    f.push(0xbeef, 16);
    f.push(0xdead, 16);
    f.push(0xdeadbeef, 32);

    f.print();

    return 0;
}
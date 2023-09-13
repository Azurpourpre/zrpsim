#include <iostream>

#include "shift.cpp"


int main(){
    Shift s = Shift(0);

    s.fill(0xffffffff);
    
    int b = 0;
    for(int i = 0; i < 8; i++){
        b = (b << 1) + s.shift(0);
    }

    std::cout << "Buffer : " << std::hex << s.get() << std::endl;
    std::cout << "b : " << b << std::endl;

    //std::cout << "shift buffer : " << std::hex << s.get() << std::endl;

    return 0;
}
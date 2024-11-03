.program io_test

loop:
    set x, 4
read:
    in pins, 1
    jmp x-- read
    push
    jmp loop
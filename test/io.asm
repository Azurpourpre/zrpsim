.program io_test

set x, 0
set y, 0
_start:
    set pins, 1
    mov pins, x
    mov x, !x
    mov pins, x
    set pins, 0 [10]

    jmp _start
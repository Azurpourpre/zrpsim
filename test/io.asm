.program io_test

set x, 0
_start:
    mov pins, x
    mov x, !x
    mov pins, x
    jmp _start
cmake_minimum_required(VERSION 3.5)

project(zrpsim)

set(LANGUAGE CXX)
set(CMAKE_BUILD_TYPE Debug)

set(SRCS
    src/shift.cpp
    src/fifo.cpp
    src/gpio.cpp
    src/main.cpp
    src/state_machine.cpp
)

add_executable(zrpsim ${SRCS})
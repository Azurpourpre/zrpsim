cmake_minimum_required(VERSION 3.5)

project(zrpsim)

set(LANGUAGE CXX)
set(CMAKE_BUILD_TYPE Debug)

set(SRCS
    src/shift.cpp
    src/main.cpp
)

add_executable(zrpsim ${SRCS})
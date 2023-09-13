cmake_minimum_required(VERSION 3.5)

project(zrpsim)

set(LANGUAGE CXX)

set(SRCS
    src/main.cpp
)

add_executable(zrpsim ${SRCS})
cmake_minimum_required(VERSION 3.5)

project(zrpsim)

set(LANGUAGE CXX)
set(CMAKE_BUILD_TYPE Debug)

set(SRCS
    src/main.cpp
)

set(HEADERS
    src/instructions.h
)

add_compile_options(-g)

add_executable(zrpsim ${SRCS} ${HEADERS})
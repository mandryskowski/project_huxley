cmake_minimum_required(VERSION 3.23)
project(armv8_25)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(armv8_25
        src/assemble.c
        src/emulate.c
        src/Makefile
        )

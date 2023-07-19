set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -pedantic")

include_directories(../)

set(SOURCES ../rbc.c ../src/Tokenizer)

project(HelloWorld)
cmake_minimum_required(VERSION 3.0)


add_executable(a ${SOURCES})
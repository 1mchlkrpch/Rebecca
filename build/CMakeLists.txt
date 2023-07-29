set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -pedantic")

include_directories(../)

set(SOURCES ../main.c ../src/Tokenizer.c ../src/Tree.c ../src/Utilities.c ../src/ParserGenerator.c ../MchlkrpchLogger/logger.c ../src/Parser_GEN_.c)

project(HelloWorld)
cmake_minimum_required(VERSION 3.0)


add_executable(rbc ${SOURCES})
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -pedantic")

include_directories(../)

set(SOURCES
	../main.c ../src/Tokenizer.c ../src/Tree.c
	../src/Utilities.c ../src/ParserGenerator2.c ../MchlkrpchLogger/logger.c
	../src/Tokenizer_GEN.c
	../src/Parser_GEN.c
	../src/Tree_GEN.c)

project(HelloWorld)
cmake_minimum_required(VERSION 3.0)


add_executable(rbc ${SOURCES})
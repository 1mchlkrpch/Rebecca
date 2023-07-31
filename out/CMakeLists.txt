set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -pedantic")

include_directories(../ ./)

set(SOURCES
	main.c
	../MchlkrpchLogger/logger.c
	Tokenizer_GEN.c
	Parser_GEN.c
	Tree_GEN.c
	)

project(HelloWorld)
cmake_minimum_required(VERSION 3.0)


add_executable(rbc ${SOURCES})
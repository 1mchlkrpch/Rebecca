# Generator of parser

That is simple tool to generate parser for arbitrary grammar.

## Structure of project

- [MchlkrpchLogger/](https://github.com/mchlkrpch/Rebecca/tree/main/MchlkrpchLogger) Simple logger to control execution of program.

- [src/](https://github.com/mchlkrpch/Rebecca/tree/main/src) All *.c files of generator.

- [include/](https://github.com/mchlkrpch/Rebecca/tree/main/include) All *.h files of generator and file with grammar: *.rbc

- [out/](https://github.com/mchlkrpch/Rebecca/tree/main/out) Folder to try out generated files:
	> Generator files:
		`Tokenizer_GEN.h`, `Tokenizer_GEN.c`, `Parser_GEN.c`, `Tree_GEN.c`
	  Visualization:
	  	`graph.dot`, `graph.png`

	### To generate these files just do:
	```c++
	git clone git@github.com:mchlkrpch/Rebecca.git
	cd Rebecca
	make init  // Create build folder
	make build // Generate 'Makefile' from cmake
	cd build   // Execute the program
	./rbc
	```
# How it works?

First of all you should choose method to set grammar. My choice is

### YACC-rules

YACC rules helps to create stable sequences of tokens...

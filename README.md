# Generator of parser

That is simple tool to generate parser for arbitrary grammar defined as sequences of tokens.

## Compile and try out first example

Let's make AST for file [examples/function.rbc](https://github.com/mchlkrpch/Rebecca/blob/main/examples/function.rbc).
1. Clone the project:
```
git clone git@github.com:mchlkrpch/Rebecca.git
cd Rebecca
```
2. Generate parser's and tokenizer's files from file [include/function.rbc](https://github.com/mchlkrpch/Rebecca/blob/main/include/function.rbc); this file defile grammar of language by [YACC-similar-rules](https://www.ibm.com/docs/en/aix/7.2?topic=information-yacc-rules)
```
make init
cd build
cmake ..
make
./rbc ../include/function.rbc
```
3. Parse the program.
```
cd ../out
make init
cd build
cmake ..
make
./rbc ../../examples/function.rbc
```
![AST](https://github.com/mchlkrpch/Rebecca/tree/main/ast_example.png?raw=true)

## Structure of project

- [MchlkrpchLogger/](https://github.com/mchlkrpch/Rebecca/tree/main/MchlkrpchLogger) Simple logger to control execution of program.

- [src/](https://github.com/mchlkrpch/Rebecca/tree/main/src) All *.c files of generator.

- [include/](https://github.com/mchlkrpch/Rebecca/tree/main/include) All *.h files of generator and file with grammar: *.rbc

- [out/](https://github.com/mchlkrpch/Rebecca/tree/main/out) Folder to try out generated files:
	> Generator files:
		`lib_GEN.h`, `Tokenizer_GEN.c`, `Parser_GEN.c`, `Tree_GEN.c`
	  Visualization:
	  	`graph.dot`, `graph.png`

# How does it work?

First of all you should choose method to set grammar. My choice is

### YACC-rules

YACC rules helps to create stable sequences of tokens.
For example here is grammar of the whole C-language: [ANSI C grammar](https://www.lysator.liu.se/c/ANSI-C-grammar-y.html)

### Specificity of rules in this project

There are several stable names:
- `%start`       Allows you to choose parent function in recursive descent.
- `%white_space` Sets whitespace symbols. These symbols tokenizer will just skip.
- `%splitters`   Set split symbols. These symbols tokenizer can notice even if you write them between two halves of the word.
- All names with symbol '_' in the end of word will be considered as [regex-expressions](https://en.wikipedia.org/wiki/Regular_expression).

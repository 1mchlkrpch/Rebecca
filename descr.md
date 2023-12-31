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

### Process of creating AST tree of the program

1. rbc reads file with grammar via it's own tokenizer.
After tokenization rbc builds AST of parser and tokenizer to generate these files.

2. rbc generates library to parse the program. It generates tokenizer file, tree commands file and parser file.

3. Now it's possible parse needed file with generated parser and tokenizer functions.
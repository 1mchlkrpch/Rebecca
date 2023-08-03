# Generator of parser

That is simple tool to generate parser for arbitrary grammar defined as sequences of tokens.

## Try out first example

Let's make AST for file [examples/function.rbc](https://github.com/mchlkrpch/Rebecca/blob/main/examples/function.rbc).
1. Clone the project:
```
git clone git@github.com:mchlkrpch/Rebecca.git
cd Rebecca
```
2. Generate parser's and tokenizer's files from file [include/function.rbc](https://github.com/mchlkrpch/Rebecca/blob/main/include/function.rbc); this file defines grammar of language by [YACC-similar-rules](https://www.ibm.com/docs/en/aix/7.2?topic=information-yacc-rules)
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
And we have AST of our program!

![ast_example](https://github.com/mchlkrpch/Rebecca/assets/86200907/74cf04e0-d837-4719-8a4f-f4aedfaba36f)

You can check details in [description file](https://github.com/mchlkrpch/Rebecca/blob/main/descr.md)

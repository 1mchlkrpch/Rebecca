#include <stdio.h>

#include <include/RebeccaCompiler.h>

int main() {
  printf("start of compiler's work\n");

  Token *sequence = Tokenizer("../examples/example1.rbc");
  (void)sequence;

  return 0;
}
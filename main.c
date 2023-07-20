#include <stdio.h>

#include <include/RebeccaCompiler.h>

int main() {
  printf("start of compiler's work\n");

  uint64_t n_tokens = 0;
  // TODO: Add analysis of tokens.
  Token *sequence = Tokenizer("../examples/example2.rbc", &n_tokens);

  for (uint64_t cur_token = 0; cur_token < n_tokens; ++cur_token) {
    printf("t(%zu)|%s -- %s\n",
      cur_token,
      sequence[cur_token].txt,
      TranslateTokenType(sequence[cur_token].type));
  }

  // TODO: Add compilation flags.
  // TODO: Add analysis of syntax tree.
  Parser *parser = BuildAst(sequence, n_tokens);
  (void)parser;

  return 0;
}
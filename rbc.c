#include <stdio.h>

#include <include/RebeccaCompiler.h>

int main() {
  printf("start of compiler's work\n");

  uint64_t n_tokens = 0;
  Token *sequence = Tokenizer("../examples/example2.rbc", &n_tokens);

  for (uint64_t cur_token = 0; cur_token < n_tokens; ++cur_token) {
    printf("t(%zu):%s\n", cur_token, sequence[cur_token].txt);
  }

  return 0;
}
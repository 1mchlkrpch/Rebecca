#include <stdio.h>

#include <include/RebeccaCompiler.h>

int main() {
  // printf("start of compiler's work\n");

  // int a = 5;
  // int *pointer = &a;
  // void *second = calloc(5, sizeof(int*));

  // int **access_ptr = (int **)second;
  // memcpy((access_ptr + 1), &pointer, sizeof(int*));

  // *(int **)((char *)second + 1 * sizeof(int)) = a;
  // memcpy(second, &pointer, sizeof(int*));
  // a = 1;
  // printf("sec:%d\n", **(access_ptr + 1));

  ///////////////////////////////////////
  uint64_t n_tokens = 0;
  // TODO: Add analysis of tokens.
  Token *sequence = Tokenizer("../include/Test.rbc", &n_tokens);
  for (uint64_t cur_token = 0; cur_token < n_tokens; ++cur_token) {
    printf("t(%zu)|%s -- %s\n",
      cur_token,
      sequence[cur_token].txt,
      TranslateTokenType(sequence[cur_token].type)
      );
  }
  GenerateParserFile(sequence, n_tokens);
  ///////////////////////////////////////

  // // Token *sequence = Tokenizer("../examples/example2.rbc", &n_tokens);
  // printf("n tokens:%lu\n", n_tokens);
  // PreParserAnalisys(sequence, n_tokens);

  // for (uint64_t cur_token = 0; cur_token < n_tokens; ++cur_token) {
  //   printf("t(%zu)|%s -- %s\n",
  //     cur_token,
  //     sequence[cur_token].txt,
  //     TranslateTokenType(sequence[cur_token].type)
  //     );
  // }


  // TODO: Add compilation flags.
  // TODO: Add analysis of syntax tree.
  // Parser *parser = BuildAst(sequence, n_tokens);
  // (void)parser;

  return 0;
}
#include <stdio.h>

#include <include/RebeccaCompiler.h>
#include <MchlkrpchLogger/logger.h>

int main() {
  ///////////////////////////////////////
  SetTabSize(2);
  __msg(D_TOKENIZER, M, "Start of tokenizer work\n");
  uint64_t n_tokens = 0;
  // TODO: Add analysis of tokens.
  Token *sequence = Tokenizer("../examples/R_differentTypes1.rbc", &n_tokens);

  __msg(D_TOKENIZER_OUTPUT, M, "--------------\n");
  __msg(D_TOKENIZER_OUTPUT, M, "Output of tokenizer:\n");
  for (uint64_t cur_token = 0; cur_token < n_tokens; ++cur_token) {
    printf(
        "t(%zu)|%s -- %s\n",
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
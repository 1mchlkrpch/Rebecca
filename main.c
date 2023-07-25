#include <stdio.h>

#include <include/RebeccaCompiler.h>
#include <MchlkrpchLogger/logger.h>

int main() {
  // SetLogfile(fopen("../logfile.txt", "w"));
  SetTabSize(2);

  __msg(D_TOKENIZER, M, "Start of tokenizer work\n");
  uint64_t n_tokens = 0;

  // TODO: Add analysis of tokens.
  Token *sequence = Tokenizer("../examples/R_differentTypes1.rbc", &n_tokens);
  
  __spt(D_TOKENIZER);
  __msg(D_TOKENIZER_OUTPUT, M, "Output of tokenizer:\n");

  for (uint64_t cur_token = 0; cur_token < n_tokens; ++cur_token) {
    __msg(D_TOKENIZER_OUTPUT, M,
      "t(%zu)|%s -- %s\n",
      cur_token,
      sequence[cur_token].txt,
      TranslateTokenType(sequence[cur_token].type));
  }

  __spt(D_TOKENIZER_OUTPUT);
  GenerateParserFile(sequence, n_tokens);
  __msg(D_PARSER_GENERATING, M,
    "End of generating parser's file\n");

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
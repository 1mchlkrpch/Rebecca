#include <stdio.h>

#include <include/RebeccaGenerator.h>
// #include <src/Parser_GEN_.h>
#include <MchlkrpchLogger/logger.h>

int main() {
  // SetLogfile(fopen("../logfile.txt", "w"));
  SetTabSize(2);

  __msg(D_TOKENIZER, M, "Start of tokenizer work\n");

  // TODO: Add analysis of tokens.
  uint64_t n_tokens = 0;
  Token *sequence = Tokenizer("../include/new_format.rbc", &n_tokens);
  
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
  // End of parser-generator's work work.
  __msg(D_PARSER_GENERATING, M,
    "End of generating parser's file\n");

  // DebugTree(parser_tree);



  // uint64_t n_tokens2 = 0;
  // Token *sequence2 = Tokenizer("../examples/if.rbc", &n_tokens2);

  // __spt(D_PARSER_GENERATING);
  // __msg(D_PARSE_EXPR, M,
  //   "Tokens:(%d)\n", n_tokens2);

  // for (uint64_t cur_token = 0; cur_token < n_tokens2; ++cur_token) {
  //   __msg(D_PARSE_EXPR, M,
  //     "t(%zu)|%s -- %s\n",
  //     cur_token,
  //     sequence2[cur_token].txt,
  //     TranslateTokenType(sequence2[cur_token].type));
  // }

  // Tree t = {0};
  // Context ctx = {0};
  // AddChild(&t, CreateNode(&t, sequence2 + n_tokens2 - 1));

  // __msg(D_PARSE_EXPR, M,
  //   "Start of executing parser!\n", n_tokens2);

  // __msg(D_PARSE_EXPR, M,
  //   "Tokens:(%d)\n", n_tokens2);

  // Try_translation_unit(&t, sequence2, ctx, n_tokens2);
  // printf("Whole the tree!\n");
  // DebugTree(&t);

  // printf("cur in seq:(%d)\n", ctx.cur_token_idx);



  // TODO: Add compilation flags.
  // TODO: Add analysis of syntax tree.
  // Parser *parser = BuildAst(sequence, n_tokens);
  // (void)parser;

  return 0;
}
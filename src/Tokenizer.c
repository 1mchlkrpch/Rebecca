#include <include/RebeccaCompiler.h>

char *GetSourceText(const char *name)
{
  assert(name != NULL && "nullptr param");

  FILE *f = fopen(name, "r");
  assert(f != NULL && "Open file error");

  fseek(f, 0, SEEK_END);
  /* Contains number of symbols wihtout EOF-symbol.
  For example with file "abaEOF" n_symbols = 3.*/
  size_t n_symbols = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *source_text = (char *)calloc(n_symbols + 1, sizeof(char));
  fread(source_text, sizeof(char), n_symbols, f);

  /* Set last character as 'EOF' symbol
  to terminate tokenizer's work*/
  source_text[n_symbols] = EOF;
  
  fclose(f);

  return source_text;
}

Token *Tokenizer(const char *name)
{
  assert(name != NULL && "nullptr param");

  char *source_text = GetSourceText(name);
  assert(source_text != NULL && "Null source text");

  char *cursor = source_text;

  uint64_t cur_token_len = 0;
  char cur_token[kTokenMaxLen] = "";

  while (*cursor != EOF) {
    cur_token[source_text - cursor]

    ++cursor;
  }

  free(source_text);

  return NULL;
}

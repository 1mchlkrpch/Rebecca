#include <include/RebeccaCompiler.h>

/**
 * @brief returns source text
 * which is come from FILE with name 'name'.
 * 
 * @param name Name of file to read.
 * @returns source_text char* text of file.
 */
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

/**
 * @brief Checks if symbol 'c' in kSplitSymbols which
 * contains all the splitter-symbols.
 * 
 * @param c Symbol to check.
 * @return true if 'c' in 'kSplitSYmbols'.
 */
static inline __attribute__((always_inline))
bool IsSplit(const char c)
{ return strchr(kSplitSymbols, c) != NULL; }

Token IdentifyToken(const char *cur_token, uint64_t cur_token_len)
{

}

void PushToken(Token token, Token *sequence)
{

}

/**
 * @brief Siple tokenizer O(n*k) where k is
 * size of keywords + splitters size.
 * 
 * Reads the file with name 'name' and
 * scans symbols from 'source_text' one by one.
 * 
 * After each split symbol checks if the current
 * set of symbols contained in 'cur_word' is keyword or another
 * stable logic-word.
 * 
 * If it isn't it will be pushed to 'sequence' as name
 * of particular variable.
 * 
 * @param name Name of file to tokenize.
 * @returns sequence of Tokens.
 */
Token *Tokenizer(const char *name)
{
  assert(name != NULL && "nullptr param");

  // Read the file with name 'name'.
  char *source_text = GetSourceText(name);
  assert(source_text != NULL && "Null source text");

  Token *sequence = (Token *)calloc(kInitSequenceSize, sizeof(Token));

  char *cursor = source_text;

  uint64_t cur_token_len = 0;
  char cur_word[kTokenMaxLen] = "";

  // Read the symbols until we meet EOF symbol.
  while (*cursor != EOF) {
    if (IsSplit(*cursor)) {
      Token token = IdentifyToken(cur_word, cur_token_len);
      PushToken(token, sequence);

      memset(cur_word, '\0', sizeof(char));
    }

    cur_word[cur_token_len] = *cursor;
    ++cur_token_len;

    ++cursor;
  }

  free(source_text);

  return sequence;
}

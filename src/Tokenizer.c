/**
 * @file Tokenizer.c
 * Tokenizer logic with reading file functions.
 */
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

/**
 * @brief Checks if symbol 'c' in kWhiteSpace which
 * contains all the whitespace-symbols.
 * 
 * @param c Symbol to check.
 * @return true if 'c' in 'kWhiteSpace'.
 */
static inline __attribute__((always_inline))
bool IsWhiteSpace(const char c)
{ return strchr(kWhiteSpace, c) != NULL; }

static const size_t kUndefinedStableWordIdx = sizeof(stable_words) / sizeof(StableWord) + 1;

/**
 * @brief Checks if the cur_word is
 * stable word or just name or number.
 * 
 * @param cur_word Word to check.
 * @returns Token to push in sequence.
 */
uint64_t IdentifyToken(char *cur_word)
{
  assert(cur_word != NULL && "nullptr param");

  for (size_t idx = 0; idx < sizeof(stable_words) / sizeof(StableWord); ++idx) {
    if (strcmp(stable_words[idx].txt, cur_word) == 0) {
      return idx;
    }
  }

  return kUndefinedStableWordIdx;
}

/**
 * @brief build token with found index
 * in 'stable_words' array.
 * 
 * @param idx Found index.
 * @returns Build token.
 */
Token ConstructToken(uint64_t idx)
{
  Token token = {0};
  
  token.txt = (char *)calloc(kTokenMaxLen, sizeof(char));
  assert(token.txt != NULL && "Null calloc allocation");


  token.type = stable_words[idx].type;
  strcpy(token.txt, stable_words[idx].txt);

  return token;
}

/**
 * @brief build token with const char* data.
 * It's soppose that token wasn't found.
 * 
 * @param txt token txt to fill.
 * @returns Build token.
 */
Token FillToken(const char *txt)
{
  assert(txt != NULL && "nullptr param");

  Token token = {0};
  
  token.txt = (char *)calloc(kTokenMaxLen, sizeof(char));
  assert(token.txt != NULL && "Null calloc allocation");

  token.type = TOKEN_NAME;
  strcpy(token.txt, txt);

  return token;
}

/**
 * @brief Adds new token 'token' to sequence of tokens 'sequence'
 * and changes sequence's size
 * 
 * @param token Token to add.
 * @param sequence Sequence to append.
 * @param sequence_size Current size of sequence of tokens.
 */
void PushToken(Token *token, Token *sequence, uint64_t *sequence_size)
{
  assert(token         != NULL && "nullptr param");
  assert(sequence      != NULL && "nullptr param");
  assert(sequence_size != NULL && "nullptr param");

  sequence[*sequence_size] = *token;
  ++(*sequence_size);
}

/**
 * @brief Tries to build token from 'cur_word'
 * and add it to token 'sequence'
 * 
 * @param cur_token_len Current size of 'cur_word'.
 * @param cur_word Current collected word.
 * @param sequence Sequence to append.
 * @param sequence_size Current size of sequence.
 */
void TryPushToken(
  uint64_t *cur_token_len, char *cur_word, Token *sequence,
  uint64_t *sequence_size)
{
  assert(cur_token_len != NULL && "nullptr param");
  assert(cur_word      != NULL && "nullptr param");
  assert(sequence      != NULL && "nullptr param");
  assert(sequence_size != NULL && "nullptr param");

  if (*cur_token_len > 0) {
    uint64_t idx = IdentifyToken(cur_word);

    Token token = {0};
    if (idx != kUndefinedStableWordIdx) {
      token = ConstructToken(idx);
    } else {
      token = FillToken(cur_word);
    }

    PushToken(&token, sequence, sequence_size);
    *cur_token_len = 0;

    memset(cur_word, '\0', kTokenMaxLen);
  }
}

/**
 * @brief Checks if the next symbol can be good
 * addition for 'cur_word' as
 *    cur_word + *(cursor + 1) -- is in stable words again.
 * Used after picking split symbol.
 * 
 * @param cur_word Current collected word.
 * @param cursor Cursor position in source text.
 * @param cur_token_len Current length of collected word.
 * 
 * @returns true if cur_word + *(cursor + 1) -- is in stable words again.
 */
bool CanBeAppended(char *cur_word, char *cursor, uint64_t cur_token_len)
{
  assert(cur_word != NULL && "nullptr param");
  assert(cursor   != NULL && "nullptr param");

  char *copy = (char *)calloc(kTokenMaxLen, sizeof(char));
  assert(copy != NULL && "Null calloc allocation");

  strcpy(copy, cur_word);

  copy[cur_token_len] = *cursor;

  if (IdentifyToken(copy) == kUndefinedStableWordIdx) {
    return false;
  } else {
    return true;
  }
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
Token *Tokenizer(const char *name, uint64_t *n_tokens)
{
  assert(name     != NULL && "nullptr param");
  assert(n_tokens != NULL && "nullptr param");

  // Read the file with name 'name'.
  char *source_text = GetSourceText(name);
  assert(source_text != NULL && "Null source text");

  // Prepare sequence.
  Token *sequence = (Token *)calloc(kInitSequenceSize, sizeof(Token));
  assert(sequence != NULL && "Null calloc allocation");

  uint64_t sequence_size = 0;

  // Prepare current word buffer.
  uint64_t cur_token_len = 0;
  char cur_word[kTokenMaxLen] = "";
  char *cursor = source_text;

  // Read the symbols until we meet EOF symbol.
  while (*cursor != EOF) {
    /* If we meet white space symbol
    -skip all white symbols
    -push current collected token if it has non-zero size.*/
    if (IsWhiteSpace(*cursor)) {
      while (IsWhiteSpace(*cursor) && *cursor != EOF) {
        ++cursor;
      }

      TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);
      continue;
    }

    /* If we have common splitter:
    -push current collected token (if it has non-zero size)
    -collect next token (it will be splitter with size 1 or 2)
    -push it to sequence.*/
    if (IsSplit(*cursor)) {
      TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);

      while (IsSplit(*cursor) != false  && *cursor != EOF) {
        cur_word[cur_token_len] = *cursor;
        ++cur_token_len;
        ++cursor;

        if (CanBeAppended(cur_word, cursor, cur_token_len)) {
          continue;
        } else {
          break;
        }
      }

      /* Try to push the biggest possible word
      collected from split symbols.*/
      TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);
      continue;
    }

    // Or we just append current word by new symbol.
    cur_word[cur_token_len] = *cursor;
    ++cur_token_len;
    ++cursor;
  }

  /* Push the last token in sequence if
  the file doesn't end with whitespace symbol.*/
  TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);
  
  // Push the final terminatie token TOKEN_EOF.
  cur_token_len = kEofTokenLength;
  strcpy(cur_word, kEofTokenTxt);
  TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);
  
  *n_tokens = sequence_size;

  free(source_text);

  return sequence;
}

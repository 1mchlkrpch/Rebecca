/**
 * @file Tokenizer.c
 * 
 * Tokenizer logic with reading file functions.
 * Also this file provides functions to check correctness of tokens in
 * source code.
 */
#include <include/RebeccaGenerator.h>
#include <MchlkrpchLogger/logger.h>

/**
 * @brief auxilary structure to create
 * cosequent array of stable words for matching
 * collected words from source code with stable words
 * in this array.
 * 
 * If tokenizer collect stabe word in 'cur_word'
 * it will matched with 'txt' in 'stable_words'-particular
 * word and tokenizer will push token with the 'txt' and 'type'
 * to token sequence.
 */
typedef struct StableWord
{
  // Text representation of token.
  const char *txt;
  // Length of token.
  size_t      len;
  // Token logic type.
  TokenType   type;
} StableWord;

#define kEofTokenTxt "EOF"
static const size_t kEofTokenLength = 3;

static const StableWord stable_words[] =
{
  {":",   1, TOKEN_COLON},
  {"\"",  1, TOKEN_DOUBLE_QUOTE},
  {"=",   1, TOKEN_EQ},

  {kEofTokenTxt, kEofTokenLength, TOKEN_EOF},

  {"%",   1, TOKEN_PERCENT},
  {"|",   1, TOKEN_PIPE},
  {";",   1, TOKEN_SEMICOLON},
  {"\'",  1, TOKEN_SINGLE_QUOTE}
};

/// @return   Next symbol.
static inline __attribute__((always_inline))
char PeekNext(char const* const cursor)
{ return *(cursor + 1); }

/// @return   Previous symbol.
static inline __attribute__((always_inline))
char PeekPrev(const char* const cursor)
{ return *(cursor - 1); }

/**
 * @brief Checks if symbol 'c' in kSplitSymbols which
 * contains all the splitter-symbols.
 * 
 * @param c   Symbol to check.
 * @return    true if 'c' in 'kSplitSYmbols'.
 */
static inline __attribute__((always_inline))
bool SplitSym(const char c)
{ return strchr(kSplitSymbols, c) != NULL; }

/**
 * @brief Checks if symbol 'c' in kWhiteSpace which
 * contains all the whitespace-symbols.
 * 
 * @param c   Symbol to check.
 * @return    true if 'c' in 'kWhiteSpace'.
 */
static inline __attribute__((always_inline))
bool WhitespaceSym(const char c)
{ return strchr(kWhiteSpace, c) != NULL; }

/**
 * @brief Checks if the symbol is the quote-symbol
 * 
 * @param c   Symbol to check.
 * @return    true if 'c' is quote symbol.
 */
static inline __attribute__((always_inline))
bool QuoteSym(const char c)
{ return c == kSingleQuoteSym || c == kDoubleQuoteSym; }

/**
 * @brief Checks if the current and next symbols are beginning of
 * the commentary block or commentary line.
 * 
 * @param cursor   Cursor in source code.
 * @returns        true if the current and next symbols is the beginning. 
 */
static inline __attribute__((always_inline))
bool CommentSyms(const char c, const char next)
{
  return
    (c == kCommentFirstSym && next == kCommentSecondSym) ||
    (c == kCommentFirstSym && next == kCommentFirstSym);
}

/**
 * @brief Auxilary function to get
 * text representation of token by token type.
 * 
 * @param type   Type of incoming token.
 * @returns      Text name of token.
 */
const char *TranslateTokenType(TokenType type)
{
  switch (type) {
    case TOKEN_COLON:             { return "TOKEN_COLON"; }
    case TOKEN_DOUBLE_QUOTE:      { return "TOKEN_DOUBLE_QUOTE"; }
    case TOKEN_EQ:                { return "TOKEN_EQ"; }
    case TOKEN_EOF:               { return "TOKEN_EOF"; }
    case TOKEN_NAME:              { return "TOKEN_NAME"; }
    case TOKEN_PERCENT:           { return "TOKEN_PERCENT"; }
    case TOKEN_PIPE:              { return "TOKEN_PIPE"; }
    case TOKEN_SEMICOLON:         { return "TOKEN_SEMICOLON"; }
    case TOKEN_SINGLE_QUOTE:      { return "TOKEN_SINGLE_QUOTE"; }
    case PARSER_TREE:             { return "PARSER_TREE"; }
    case TOKENIZER_TREE:          { return "TOKENIZER_TREE"; }

    default: {
      return "TOKEN_UNKNOWN";
    }
  }

  return "TOKEN_UNKNOWN";
}

static const size_t kNotFoundIdx = sizeof(stable_words) / sizeof(StableWord) + 1;

/**
 * @brief Checks if the cur_word is
 * stable word.
 * 
 * @param cur_word   Word to check.
 * @returns          Token to push in sequence.
 */
static uint64_t IdentifyType(char *word)
{
  assert(word != NULL && "nullptr param");

  for (size_t idx = 0; idx < sizeof(stable_words) / sizeof(StableWord); ++idx) {
    if (strcmp(stable_words[idx].txt, word) == 0) {
      return idx;
    }
  }

  return kNotFoundIdx;
}

/**
 * @brief build token with found index
 * in 'stable_words' array.
 * 
 * @param idx   Found index.
 * @returns     Build token.
 */
static Token CtorTokenByType(uint64_t idx)
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
 * @param txt   token txt to fill.
 * @returns     Build token.
 */
static Token CtorToken(const char *txt)
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
 * @param token           Token to add.
 * @param sequence        Sequence to append.
 * @param sequence_size   Current size of sequence of tokens.
 */
static void PushToken(Token *token, Token *sequence, uint64_t *sequence_size)
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
 * @param word_sz   Current size of 'cur_word'.
 * @param cur_word        Current collected word.
 * @param sequence        Sequence to append.
 * @param sequence_size   Current size of sequence.
 */
static void TryPush
    (uint64_t *word_sz, char *cur_word, Token *sequence, uint64_t *sequence_size)
{
  assert(word_sz != NULL && "nullptr param");
  assert(cur_word      != NULL && "nullptr param");
  assert(sequence      != NULL && "nullptr param");
  assert(sequence_size != NULL && "nullptr param");

  msg(D_TOKENIZER, M, "Try push token\"%s\"\n", cur_word);
  tab_incr();
  if (*word_sz > 0) {
    uint64_t idx = IdentifyType(cur_word);

    Token token = {0};
    if (idx != kNotFoundIdx) {
      token = CtorTokenByType(idx);
    } else {
      token = CtorToken(cur_word);
    }
    msg(D_TOKENIZER, M, "token filled\n");

    PushToken(&token, sequence, sequence_size);
    *word_sz = 0;

    memset(cur_word, '\0', kTokenMaxLen);
    msg(D_TOKENIZER, M, "now cur_word:\"%s\"-empty\n", cur_word);
    
    tab_decr();
    msg(D_TOKENIZER, M, "Token:\"%s\" - pushed\n", token.txt);
  } else {
    tab_decr();
    msg(D_TOKENIZER, M, "Not pushed\n", cur_word);
  }
}

/**
 * @brief Moves cursor untill the cursor
 * peek not commentary block.
 * 
 * @param cursor   Pointer to cursor in source text.
 */
static void SkipCommentary(char const **cursor)
{
  ++(*cursor);

  if (**cursor != EOF && **cursor == kCommentSecondSym) {
    msg(D_TOKENIZER, M,
      "Waiting for long commentary-end symbol\n");

    while (**cursor != kCommentFirstSym || PeekPrev(*cursor) != kCommentSecondSym) {
      ++(*cursor);
    }

    ++(*cursor);
  } else if (**cursor != EOF && **cursor == kCommentFirstSym) {
    msg(D_TOKENIZER, M, "Waiting for small commentary-end symbol\n");
    while (**cursor != EOF && **cursor != kNewLineSym){
      ++(*cursor);
    }

    ++(*cursor);
  }

  msg(D_TOKENIZER, M, "Commentary ends\n");
}

static inline __attribute__((always_inline))
void AppendWord(char *word, uint64_t *word_sz, char const **cursor)
{
  word[*word_sz] = **cursor;
  //  Moves pointer and
  // increase size of word.
  ++(*word_sz);
  ++(*cursor);
}

static void CollectQuotedData
    (char const **cursor, char *word, uint64_t *word_sz, uint64_t *n_tokens, Token *sequence)
{
  bool is_prev_escape_sym = false;
  // Check if previous character was escaped.
  while ((!QuoteSym(**cursor) || is_prev_escape_sym == true) && **cursor != EOF) {
    // Set next symbol escaped.
    if (**cursor == kEscapeSym) {
      is_prev_escape_sym = true;
      AppendWord(word, word_sz, cursor);
    }

    if (is_prev_escape_sym == true && PeekPrev(*cursor) == kEscapeSym) {
      is_prev_escape_sym = false;
    }

    AppendWord(word, word_sz, cursor);
  }

  TryPush(word_sz, word, sequence, n_tokens);
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
 * @param name   Name of file to tokenize.
 * @returns      sequence of Tokens.
 */
Token *Tokenizer(char const* const txt, uint64_t *n_tokens)
{
  assert(txt      != NULL && "nullptr param");
  assert(n_tokens != NULL && "nullptr param");

  Token *sequence = (Token*)calloc(kInitSequenceSize, sizeof(Token));
  assert(sequence != NULL && "Null calloc allocation");

  uint64_t word_sz = 0;
  char word[kTokenMaxLen] = "";

  //  Pointer to current symbol in source text (txt).
  char const *cursor = txt;

  // Read the symbols until we meet EOF.
  while (*cursor != EOF) {
    msg(D_TOKENIZER, M, "sym:(%c)\n", *cursor);
    
    //  Skip comment if it is.
    // (word_sz) > 0 means that it can be divide-symbol.
    if (CommentSyms(*cursor, PeekNext(cursor)) && word_sz == 0) {
      SkipCommentary(&cursor);
      continue;
    }

    // If it's whitespace symbol
    if (WhitespaceSym(*cursor)) {
      // Skip All whitespace symbols.
      while (WhitespaceSym(*cursor) && *cursor != EOF) {
        ++cursor;
      }

      // Push collected word.
      TryPush(&word_sz, word, sequence, n_tokens);
      continue;
    }

    // If it's split symbol
    if (SplitSym(*cursor)) {
      // Push collected word.
      TryPush(&word_sz, word, sequence, n_tokens);

      // It can be a comment symbol.
      if (CommentSyms(*cursor, PeekNext(cursor))) {
        continue;
      }

      // It can be a quote symbol.
      if (QuoteSym(*cursor)) {
        // Push Quote.
        AppendWord(word, &word_sz, &cursor);
        TryPush(&word_sz, word, sequence, n_tokens);

        // Сollect string between quotes.
        CollectQuotedData(&cursor, word, &word_sz, n_tokens, sequence);

        // Push Quote.
        AppendWord(word, &word_sz, &cursor);
        TryPush(&word_sz, word, sequence, n_tokens);
        continue;
      }

      // Push collected word.
      TryPush(&word_sz, word, sequence, n_tokens);

      // Push split.
      AppendWord(word, &word_sz, &cursor);
      TryPush(&word_sz, word, sequence, n_tokens);
      continue;
    }

    // Or we just append current word by new symbol.
    AppendWord(word, &word_sz, &cursor);
  }

  //  Push collected word.
  // Which could be collected before EOF.
  TryPush(&word_sz, word, sequence, n_tokens);
  
  // Push TOKEN_EOF.
  word_sz = kEofTokenLength;
  strcpy(word, kEofTokenTxt);
  TryPush(&word_sz, word, sequence, n_tokens);

  return sequence;
}

#undef kEofTokenTxt

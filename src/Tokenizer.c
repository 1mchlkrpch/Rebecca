/**
 * @file Tokenizer.c
 * 
 * Tokenizer logic with reading file functions.
 * Also this file provides functions to check correctness of tokens in
 * source code.
 */
#include <include/RebeccaCompiler.h>
#include <MchlkrpchLogger/logger.h>

/**
 * @brief returns source text
 * which is come from FILE with name 'name'.
 * 
 * @param     name Name of file to read.
 * @returns   source_text char* text of file.
 */
char *GetSourceText(const char *name)
{
  assert(name != NULL && "nullptr param");
  
  __tab_incr();

  FILE *f = fopen(name, "r");
  assert(f != NULL && "Open file error");

  fseek(f, 0, SEEK_END);
  /* Contains number of symbols wihtout EOF-symbol.
  For example with file "abaEOF" n_symbols = 3.*/
  size_t n_symbols = ftell(f);
  __msg(D_TOKENIZER, M, "mush be allocated:%zu\n", n_symbols);
  fseek(f, 0, SEEK_SET);

  char *source_text = (char *)calloc(n_symbols + 1, sizeof(char));
  fread(source_text, sizeof(char), n_symbols, f);

  /* Set last character as 'EOF' symbol
  to terminate tokenizer's work*/
  source_text[n_symbols] = EOF;
  
  fclose(f);

  __tab_decr();

  return source_text;
}

/**
 * @brief auxilary function to get
 * text representation of token by token type.
 * 
 * @param type     Type of incoming token.
 * @returns Text   name of token.
 */
const char *TranslateTokenType(TokenType type)
{
  switch (type) {
    case TOKEN_UNKNOWN:             { return "UNKNOWN"; }
    case TOKEN_LEFT_PARENTHESIS:    { return "TOKEN_LEFT_PARENTHESIS"; }
    case TOKEN_RIGHT_PARENTHESIS:   { return "TOKEN_RIGHT_PARENTHESIS"; }
    case TOKEN_LEFT_BRACKET:        { return "TOKEN_LEFT_BRACKET"; }
    case TOKEN_RIGHT_BRACKET:       { return "TOKEN_RIGHT_BRACKET"; }
    case TOKEN_LEFT_BRACE:          { return "TOKEN_LEFT_BRACE"; }
    case TOKEN_RIGHT_BRACE:         { return "TOKEN_RIGHT_BRACE"; }
    case TOKEN_COLON:               { return "TOKEN_COLON"; }
    case TOKEN_SEMICOLON:           { return "TOKEN_SEMICOLON"; }
    case TOKEN_DOT:                 { return "TOKEN_DOT"; }
    case TOKEN_COMMA:               { return "TOKEN_COMMA"; }
    case TOKEN_SINGLE_QUOTE:        { return "TOKEN_SINGLE QUOTE"; }
    case TOKEN_DOUBLE_QUOTE:        { return "TOKEN_DOUBLE QUOTE"; }
    case TOKEN_STAR:                { return "TOKEN_STAR"; }
    case TOKEN_SLASH:               { return "TOKEN_SLASH"; }
    case TOKEN_BACK_SLASH:          { return "TOKEN_BACK_SLASH"; }
    case TOKEN_PERCENT:             { return "TOKEN_PERCENT"; }
    case TOKEN_HASHTAG:             { return "TOKEN_HASHTAG"; }
    case TOKEN_PLUS:                { return "TOKEN_PLUS"; }
    case TOKEN_PLUSPLUS:            { return "TOKEN_PLUSPLUS"; }
    case TOKEN_MINUS:               { return "TOKEN_MINUS"; }
    case TOKEN_LL:                  { return "TOKEN_LL"; }
    case TOKEN_GG:                  { return "TOKEN_GG"; }
    case TOKEN_PIPE:                { return "TOKEN_PIPE"; }
    case TOKEN_PIPEPIPE:            { return "TOKEN_PIPEPIPE"; }
    case TOKEN_CARET:               { return "TOKEN_CARET"; }
    case TOKEN_TILDE:               { return "TOKEN_TILDE"; }
    case TOKEN_QUESTION:            { return "TOKEN_QUESTION"; }
    case TOKEN_EXCLAMATION:         { return "TOKEN_EXCLAMATION"; }
    case TOKEN_EQ:                  { return "TOKEN_EQ"; }
    case TOKEN_L:                   { return "TOKEN_L"; }
    case TOKEN_G:                   { return "TOKEN_G"; }
    case TOKEN_LEQ:                 { return "TOKEN_LEQ"; }
    case TOKEN_GEQ:                 { return "TOKEN_GEQ"; }
    case TOKEN_EQEQ:                { return "TOKEN_EQEQ"; }
    case TOKEN_EXCLAMATION_EQ:      { return "TOKEN_EXCLAMATION_EQ"; }
    case TOKEN_BREAK:               { return "TOKEN_BREAK"; }
    case TOKEN_CONTINUE:            { return "TOKEN_CONTINUE"; }
    case TOKEN_CLASS:               { return "TOKEN_CLASS"; }
    case TOKEN_STRUCT:              { return "TOKEN_STRUCT"; }
    case TOKEN_ELSE:                { return "TOKEN_ELSE"; }
    case TOKEN_FALSE:               { return "TOKEN_FALSE"; }
    case TOKEN_CYCLE:               { return "TOKEN_CYCLE"; }
    case TOKEN_IF:                  { return "TOKEN_IF"; }
    case TOKEN_LOAD:                { return "TOKEN_LOAD"; }
    case TOKEN_NULL:                { return "TOKEN_NULL"; }
    case TOKEN_RETURN:              { return "TOKEN_RETURN"; }
    case TOKEN_STATIC:              { return "TOKEN_STATIC"; }
    case TOKEN_THIS:                { return "TOKEN_THIS"; }
    case TOKEN_TRUE:                { return "TOKEN_TRUE"; }
    case TOKEN_PRIVATE:             { return "TOKEN_PRIVATE"; }
    case TOKEN_PUBLIC:              { return "TOKEN_PUBLIC"; }
    case TOKEN_NAME:                { return "TOKEN_NAME"; }
    case TOKEN_NUMBER:              { return "TOKEN_NUMBER"; }
    case TOKEN_COMP:                { return "TOKEN_COMP"; }
    case TOKEN_UNDERLINE:           { return "TOKEN_UNDERLINE"; }
    case TOKEN_ABSTRACT_TYPE:       { return "TOKEN_ABSTRACT_TYPE"; }
    case TOKEN_EOF:                 { return "TOKEN_EOF"; }
    default: {
      return "UNKNOWN";
    }
  }

  return "UNKNOWN";
}


static const size_t kUndefinedStableWordIdx = sizeof(stable_words) / sizeof(StableWord) + 1;

/**
 * @brief Checks if symbol 'c' in kSplitSymbols which
 * contains all the splitter-symbols.
 * 
 * @param c   Symbol to check.
 * @return    true if 'c' in 'kSplitSYmbols'.
 */
static inline __attribute__((always_inline))
bool IsSplit(const char c)
{ return strchr(kSplitSymbols, c) != NULL; }

/**
 * @brief Checks if symbol 'c' in kWhiteSpace which
 * contains all the whitespace-symbols.
 * 
 * @param c   Symbol to check.
 * @return    true if 'c' in 'kWhiteSpace'.
 */
static inline __attribute__((always_inline))
bool IsWhiteSpace(const char c)
{ return strchr(kWhiteSpace, c) != NULL; }

/**
 * @brief Checks if symbol 'c' in kDigitSymbols which
 * contains all digit-symbols.
 * 
 * @param c   Symbol to check.
 * @return    true if 'c' in 'kDigitSymbols'.
 */
static inline __attribute__((always_inline))
bool IsDigit(const char c)
{ return strchr(kDigitSymbols, c) != NULL; }

/**
 * @brief Checks if the cur_word is
 * stable word or just name or number.
 * 
 * @param cur_word   Word to check.
 * @returns          Token to push in sequence.
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
 * @param idx   Found index.
 * @returns     Build token.
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
 * @param txt   token txt to fill.
 * @returns     Build token.
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
 * @param token           Token to add.
 * @param sequence        Sequence to append.
 * @param sequence_size   Current size of sequence of tokens.
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
 * @param cur_token_len   Current size of 'cur_word'.
 * @param cur_word        Current collected word.
 * @param sequence        Sequence to append.
 * @param sequence_size   Current size of sequence.
 */
void TryPushToken(
  uint64_t *cur_token_len, char *cur_word, Token *sequence, uint64_t *sequence_size)
{
  assert(cur_token_len != NULL && "nullptr param");
  assert(cur_word      != NULL && "nullptr param");
  assert(sequence      != NULL && "nullptr param");
  assert(sequence_size != NULL && "nullptr param");

  __msg(D_TOKENIZER, M, "Try push token\"%s\"\n", cur_word);
  __tab_incr();
  if (*cur_token_len > 0) {
    uint64_t idx = IdentifyToken(cur_word);

    Token token = {0};
    if (idx != kUndefinedStableWordIdx) {
      token = ConstructToken(idx);
    } else {
      token = FillToken(cur_word);
    }
    __msg(D_TOKENIZER, M, "token filled\n");

    PushToken(&token, sequence, sequence_size);
    *cur_token_len = 0;

    memset(cur_word, '\0', kTokenMaxLen);
    __msg(D_TOKENIZER, M, "now cur_word:\"%s\"-empty\n", cur_word);
    
    __tab_decr();
    __msg(D_TOKENIZER, M, "Token:\"%s\" - pushed\n", token.txt);
  } else {
    __tab_decr();
    __msg(D_TOKENIZER, M, "Not pushed\n", cur_word);
  }
}

/**
 * @brief Checks if the next symbol can be good
 * addition for 'cur_word' as
 *    cur_word + *(cursor + 1) -- is in stable words again.
 * Used after picking split symbol.
 * 
 * @param cur_word        Current collected word.
 * @param cursor Cursor   position in source text.
 * @param cur_token_len   Current length of collected word.
 * @returns               true if cur_word + *(cursor + 1) -- is in stable words again.
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
 * @brief Reads the value of current number-token.
 * 
 * @param cursor          Pointer to pointer in source text.
 * @param cur_word        Current collected word.
 * @param cur_token_len   Pointer to current length of collected token.
 * @returns               Value of number.
 */
uint64_t ReadNumber(char **cursor, char *cur_word, uint64_t *cur_token_len)
{
  assert(cursor        != NULL && "nullptr param");
  assert(cur_word      != NULL && "nullptr param");
  assert(cur_token_len != NULL && "nullptr param");

  __tab_incr();
  __msg(D_TOKENIZER, M, "Reading number:\n");
  __tab_incr();

  __msg(D_TOKENIZER, M, "(");
  while (IsDigit(**cursor) && **cursor != EOF) {
    __msg(D_TOKENIZER, ' ', "%c", **cursor);
    cur_word[*cur_token_len] = **cursor;
    ++(*cur_token_len);
    ++(*cursor);
  }

  __msg(D_TOKENIZER, ' ', ")\n");

  __tab_decr();
  __msg(D_TOKENIZER, M, "number done\n");
  __tab_decr();

  return atoi(cur_word);
}

/**
 * @brief Checks if the current and next symbols are beginning of
 * the commentary block or commentary line.
 * 
 * @param cursor   Cursor in source code.
 * @returns        true if the current and next symbols is the beginning. 
 */
bool CheckIfItsCommentary(char *cursor)
{ return (*cursor == '/' && *(cursor + 1) == '*') || (*cursor == '/' && *(cursor + 1) == '/'); }

/**
 * @brief Moves cursor untill the cursor
 * peek not commentary block.
 * 
 * @param cursor   Pointer to cursor in source text.
 */
void SkipCommentary(char **cursor)
{
  ++(*cursor);

  if (**cursor != EOF && **cursor == '*') {
    __msg(D_TOKENIZER, M, "Waiting for long commentary-end symbol\n");
    while (**cursor != '/' || *(*cursor - 1) != '*') {
      ++(*cursor);
    }

    ++(*cursor);
  } else if (**cursor != EOF && **cursor == '/') {
    __msg(D_TOKENIZER, M, "Waiting for small commentary-end symbol\n");
    while (**cursor != EOF && **cursor != '\n'){
      ++(*cursor);
    }

    ++(*cursor);
  }

  __msg(D_TOKENIZER, M, "Commentary ends\n");
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
Token *Tokenizer(const char *name, uint64_t *n_tokens)
{
  assert(name     != NULL && "nullptr param");
  assert(n_tokens != NULL && "nullptr param");

  // Read the file with name 'name'.
  __msg(D_TOKENIZER, M, "Read text\n");
  char *source_text = GetSourceText(name);
  assert(source_text != NULL && "Null source text");
  __msg(D_TOKENIZER, M, "Source text successfully copied!\n");

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
    // Debug...
    if (*cursor == '\n') { __msg(D_TOKENIZER, M, "sym:\'%s\'\n", "\\n"); }
    else                 { __msg(D_TOKENIZER, M, "sym:\'%c\'\n", *cursor); }
    /* Checks if current symbol
    is the beginning of the commentary block of cosequent types:
      one-string commentary: //...
      several-strings commentary: /'star'...'star'/
    And then skip commentary in 'SkipCommentary function'.*/
    if (CheckIfItsCommentary(cursor) && cur_token_len == 0) {
      __tab_incr();
      __msg(D_TOKENIZER, M, "Skip commentary\n");
      SkipCommentary(&cursor);
      __tab_decr();
      continue;
    }

    if (IsDigit(*cursor) && cur_token_len == 0) {
      __tab_incr();
      __msg(D_TOKENIZER, M, "Current token is digit value\n");

      int num = ReadNumber(&cursor, cur_word, &cur_token_len);
      TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);

      sequence[sequence_size - 1].type = TOKEN_NUMBER;

      // Adding a value manually.
      // TODO: fix later.
      sequence[sequence_size - 1].value = (Value*)calloc(1, sizeof(Value));
      __asrt(sequence[sequence_size - 1].value != NULL, "Null calloc allocation");
      sequence[sequence_size - 1].value->obj = calloc(1, sizeof(int));
      sequence[sequence_size - 1].value->type = OBJ_INT;
      memcpy(&sequence[sequence_size - 1].value->obj, &num, sizeof(int));

      __tab_decr();
      continue;
    }

    /* If we meet white space symbol
    -skip all white symbols
    -push current collected token if it has non-zero size.*/
    if (IsWhiteSpace(*cursor)) {
      __tab_incr();
      __msg(D_TOKENIZER, M, "Whitespace symbol; try to push collected token\n");
      while (IsWhiteSpace(*cursor) && *cursor != EOF) {
        ++cursor;
      }

      TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);
      __tab_decr();
      continue;
    }

    /* If we have common splitter:
    -push current collected token (if it has non-zero size)
    -collect next token (it will be splitter with size 1 or 2)
    -push it to sequence.*/
    if (IsSplit(*cursor)) {
      __msg(D_TOKENIZER, M, "Split symbol; try to push collected token\n");
      if (CheckIfItsCommentary(cursor)) {
        TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);
        continue;
      }

      __msg(D_TOKENIZER, M, "Push current token with data \"%s\"\n", cur_word);
      TryPushToken(&cur_token_len, cur_word, sequence, &sequence_size);

      while (IsSplit(*cursor)  && *cursor != EOF) {
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

/**
 * @brief fast analisys targeted to identifier types of tokens.
 * 
 * @param sequence   Sequence of token to analize.
 * @param n_tokens   size of sequence(in tokens).
 */
void PreParserAnalisys(Token *sequence, uint64_t n_tokens)
{
  assert(sequence != NULL && "Null param");

  if (n_tokens == 0) {
    return;
  }

  for (size_t cur_token = 0; cur_token < n_tokens - 1; ++cur_token) {
    if (sequence[cur_token].type == TOKEN_NAME && sequence[cur_token + 1].type == TOKEN_NAME) {
      sequence[cur_token].type = TOKEN_ABSTRACT_TYPE;
    }
  }
}

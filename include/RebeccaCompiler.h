/**
 * @file RebeccaCompiler.h
 * Main compiler logic of Rebecca-language.
 */
#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// #include <include/>

#define kTokenMaxLen 256
static const uint64_t kMaxLocals     = 256;
static const uint64_t kMaxScopeDepth = 256;
static const uint64_t kInitSequenceSize = 256;

/**
 * @brief Logically different token types.
 * 
 * @note For example '|' and '||' is one logical
 * token and they have different meanings.
 */
typedef enum
{
  TOKEN_LEFT_PARENTHESIS,   // '('
  TOKEN_RIGHT_PARENTHESIS,  // ')'
  TOKEN_LEFT_BRACKET,       // '['
  TOKEN_RIGHT_BRACKET,      // ']'
  TOKEN_LEFT_BRACE,         // '{'
  TOKEN_RIGHT_BRACE,        // '}'
  TOKEN_COLON,              // ':'
  TOKEN_DOT,                // '.'
  TOKEN_COMMA,              // ','
  TOKEN_STAR,               // '*'
  TOKEN_SLASH,              // '/'
  TOKEN_BACK_SLASH,         // '\'
  TOKEN_PERCENT,            // '%'
  TOKEN_HASHTAG,            // '#'
  TOKEN_PLUS,               // '+'
  TOKEN_MINUS,              // '-'
  TOKEN_LL,                 // '<<'
  TOKEN_GG,                 // '<<'
  TOKEN_PIPE,               // '|'
  TOKEN_PIPEPIPE,           // '||'
  TOKEN_CARET,              // '^'
  TOKEN_TILDE,              // '~'
  TOKEN_QUESTION,           // '?'
  TOKEN_EXCLAMATION,        // '!'
  TOKEN_EQ,                 // '='
  TOKEN_L,                  // '<'
  TOKEN_G,                  // '>'
  TOKEN_LEQ,                // '<='
  TOKEN_GEQ,                // '>='
  TOKEN_EQEQ,               // '=='
  TOKEN_EXCLAMATION_EQ,     // '!='

  TOKEN_BREAK,              // 'break'
  TOKEN_CONTINUE,           // 'continue'
  TOKEN_CLASS,              // 'class'
  TOKEN_STRUCT,             // 'struct'
  TOKEN_ELSE,               // 'else'
  TOKEN_FALSE,              // 'false'
  TOKEN_CYCLE,              // 'for'
  TOKEN_IF,                 // 'if'
  TOKEN_LOAD,               // 'load'
  TOKEN_NULL,               // 'nullptr'
  TOKEN_RETURN,             // 'ret'
  TOKEN_STATIC,             // 'static'
  TOKEN_THIS,               // 'this'
  TOKEN_TRUE,               // 'true'

  TOKEN_PRIVATE,            // 'private'
  TOKEN_PUBLIC,             // 'public'
  TOKEN_NAME,               // any name of variavle/function/class/...
  TOKEN_NUMBER,             // any number for example: 123

  TOKEN_UNDERLINE,          // '_'
  
  TOKEN_EOF                 // end of file
} TokenType;

typedef struct
{
  const char *txt;
  size_t      len;
  TokenType   type;
} Keyword;

static Keyword keywords[] =
{
  {"break",     5, TOKEN_BREAK},
  {"continue",  8, TOKEN_CONTINUE},
  {"class",     5, TOKEN_CLASS},
  {"else",      4, TOKEN_ELSE},
  {"false",     5, TOKEN_FALSE},
  {"cycle",     5, TOKEN_CYCLE},
  {"if",        2, TOKEN_IF},
  {"load",      4, TOKEN_LOAD},
  {"null",      4, TOKEN_NULL},
  {"public",    6, TOKEN_PUBLIC},
  {"private",   7, TOKEN_PRIVATE},
  {"ret",       3, TOKEN_RETURN},
  {"static",    6, TOKEN_STATIC},
  {"this",      4, TOKEN_THIS},
  {"true",      4, TOKEN_TRUE},
  {NULL,        0, TOKEN_EOF}
};

static const char *kSplitSymbols = " \n\t";

/**
 * @addtogroup Variables
 * @{
 */
typedef struct
{
  int val;
} Value;

typedef enum
{
  // Local scope.
  SCOPE_LOCAL,
  // Upper scope.
  SCOPE_UPPER,
  // Global scope.
  SCOPE_GLOBAL
} Scope;

/**
 * @}
 */

typedef struct
{
  // Type of token.
  TokenType type;
  // Static value (if exists).
  Value     value;

  // Number to identify token.
  uint64_t identifier;
} Token;

typedef struct
{
  const char *name;

  uint64_t *depth;
} Context;

/**
 * @brief parser machine.
 * Uses Token 'sequence' and build the AST.
 */
typedef struct
{
  // Tokens from tokenizer.
  Token *sequence;
  // Current context on each token.
  Context ctx;
} Parser;

Token *Tokenizer(const char *name);
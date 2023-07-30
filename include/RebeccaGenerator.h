/**
 * @file RebeccaCompiler.h
 * Main compiler logic of Rebecca-language.
 * 
 * Provides
 *   -main constants,
 *   -token identifiers,
 *   -token text representation,
 *   -all functions of each part of compilation (from frontend to binary translation)
 */
#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <include/Utilities.h>
#include <include/Value.h>

// Global constants --------------------------------------------------------------------------

/// Maximal token length in characters.
#define kTokenMaxLen 256

/// Maximal local variables in scope restriction.
static const uint64_t kMaxLocals     = 256;
/// Maximal scope layers in program to restrict memory consuming.
static const uint64_t kMaxScopeDepth = 256;
/// Maximal scope layers in program.
static const uint64_t kInitSequenceSize = 1024;

/**
 * @brief Logically different token types.
 * 
 * @note For example '|' and '||' is one logical
 * token and they have different meanings.
 */
typedef enum TokenType
{
  TOKEN_UNKNOWN,
  TOKEN_COLON,              // ':'
  TOKEN_DOUBLE_QUOTE,       // '"'
  TOKEN_EQ,                 // '='
  TOKEN_EOF,                // end of file
  TOKEN_HASHTAG,            // '#'
  TOKEN_LEFT_PARENTHESIS,   // '('
  TOKEN_NAME,               // any name of variavle/function/class/...
  TOKEN_PERCENT,            // '%'
  TOKEN_PIPE,               // '|'
  TOKEN_RIGHT_PARENTHESIS,  // ')'
  TOKEN_SEMICOLON,          // ';'
  TOKEN_SINGLE_QUOTE,       // '''
  // Auxilary token types for tree representation:
  TOKENIZER_TREE,
  PARSER_TREE
} TokenType;

// TOKEN_UNKNOWN
// TOKEN_COLON
// TOKEN_DOUBLE_QUOTE
// TOKEN_EQ
// TOKEN_EOF
// TOKEN_HASHTAG
// TOKEN_LEFT_PARENTHESIS
// TOKEN_NAME
// TOKEN_PERCENT
// TOKEN_PIPE
// TOKEN_RIGHT_PARENTHESIS
// TOKEN_SEMICOLON
// TOKEN_SINGLE_QUOTE 

const char *TranslateTokenType(TokenType type);

// Lexer part --------------------------------------------------------------------------------
/**
 * @addtogroup Lexer
 * Simple Lexer realization.
 * @{
 */

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
typedef struct
{
  // Text representation of token.
  const char *txt;
  // Length of token.
  size_t      len;
  // Token logic type.
  TokenType   type;
} StableWord;

static const char  *kEofTokenTxt = "EOF";
static const size_t kEofTokenLength = 3;

static StableWord stable_words[] =
{
  {":",   1, TOKEN_COLON},
  {"\"",  1, TOKEN_DOUBLE_QUOTE},
  {"=",   1, TOKEN_EQ},
  {"EOF", 3, TOKEN_EOF},
  {"#",   1, TOKEN_HASHTAG},
  {"(",   1, TOKEN_LEFT_PARENTHESIS},
  {")",   1, TOKEN_NAME},
  {"%",   1, TOKEN_PERCENT},
  {"|",   1, TOKEN_PIPE},
  {")",   1, TOKEN_RIGHT_PARENTHESIS},
  {";",   1, TOKEN_SEMICOLON},
  {"\'",  1, TOKEN_SINGLE_QUOTE}
};

// Split symbols to split words in source code.
static const char *kSplitSymbols = "():;%|=\'\"";
// Whitespace symbols in source code.
static const char *kWhiteSpace   = " \n\t";
// Digit symbols in source code.
static const char *kDigitSymbols = "01234567890";
// Commentary symbols in source code.
static const char *kCommentarySymbols = "/";
/**
 * @}
 */

// Types of node for parser.
typedef enum
{
  NOT_SPECIAL,
   VAR_NAME,
  RULE_NAME,
  RULE_NAME_REFERENCE,
   VAR_NAME_REFERENCE,
} PrsrNdType;

typedef struct
{
  // ---------------- This part fill the tokenizer
  // Type of token.
  TokenType type;
  // Text representation of token.
  char *txt;
  // Static value (if exists).
  Value *value;
  PrsrNdType parser_type;
} Token;

typedef struct
{
  uint64_t n_parsed;
  uint64_t cur_token_idx;
} Context;

// Check src/Tokenizer.c
Token *Tokenizer(const char *name, uint64_t *n_tokens);

/**
 * @}
 */

// AST part ----------------------------------------------------------------------------------
// Format of node to print it in graphviz.
#define NODE_FMT                                                                            \
  "\tn%lu [shape=\"%s\" color=\"%s\" label=\"(%s)\\n%s\"]\n"

/**
 * @brief Node of abstract syntax tree
 * structure for ast representation.
 */
typedef struct Node
{
  // Children of particular node.
  Array       *children;
  // Token of current cell in AST.
  Token       *token;
  // Numbered-id for each node.
  uint64_t     id;
  // Parent node for current one.
  struct Node *parent;
  bool         rule_name;
  // Type of node in parser generating.
} Node;

// typedef struct Node Node;

/**
 * @brief abstract syntax tree
 * structure. The next repsesentation
 * of source code after token sequence.
 * 
 * After buildint abstract syntax tree
 * we can easily check it for errors and print out
 * mistake.
 * 
 * After translating token sequence to AST
 * it will be easy to generate IR code from it
 * and optimize.
 */
typedef struct
{
  // Root node of AST.
  Node       *root;
  Node       *current;
  // Number of nodes in AST.
  size_t      size;
  const char *data;
} Tree;

Tree *GenerateParserAst(Tree *parser_tree, Tree *tokenizer_tree, Token *sequence, uint64_t n_tokens);
// YACC-simmilar tiny ast-builder.
// Tree *GenerateParserAst(Token *sequence, uint64_t n_tokens);
// Translate YACC-similar file to parser file.
// Tree *GenerateParserFile(Token *sequence, uint64_t n_tokens);
void GenerateParserFile(Token *sequence, uint64_t n_tokens);

Node *AddChild(Tree *t, Node *n);

Node *GetChild(Node *n, uint64_t idx);

void FillCurrent(Tree *t, Token *token);

void DebugTree(Tree *t);

void Parent(Tree *t);

Node *NodeCtor();
Tree *TreeCtor();

void AppendTree(Tree *first, Tree *second);

void FillNodeWith(Token *token);

/**
 * @brief parser machine.
 * Uses Token 'sequence' and build the AST.
 */
typedef struct
{
  // Tokens from tokenizer.
  Tree *tree;
  // Current context on each token.
  Context ctx;
  // Index of current token to parse in sequence.
  Token *current_token;
  // Sequence to parse.
  Token *sequence;
} Parser;

// Check src/Parser.c
Parser *BuildAst(Token *sequence, uint64_t n_tokens);

void InsertParent(Tree *t, Node *n);

void PrintNode(FILE *f, Node *n);

Node *CreateNode(Tree *t, Token *token);

Node *CreateNodeByType(Tree *t, TokenType type);

void PreParserAnalisys(Token *sequence, uint64_t n_tokens);

static const size_t kInitSizeNamesArray = 512;

typedef struct
{
  size_t size;
  Token *names;
} NameTable;

#pragma GCC diagnostic pop


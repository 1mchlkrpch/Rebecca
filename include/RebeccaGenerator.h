#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <include/Utilities.h>

// Global entities --------------------------------------------------------------------------
/// Maximal token length in characters.
#define kTokenMaxLen 256
/// Initial size of sequence of token sequence.
static const uint64_t kInitSequenceSize = 1024;
/// Name in table wasn't found index
static const int64_t kUndefinedIdx = -1;
/**
 * @brief Types of tokens
 * in tokenizing/parsing of parser's AST parseing.
 */
typedef enum TokenType
{
  /// Default type.
  TOKEN_UNKNOWN,
  // Rule start token: (:)
  TOKEN_COLON,
  // Variable string data: (")
  TOKEN_DOUBLE_QUOTE,
  // Variable equality symbol: (=)
  TOKEN_EQ,
  // EOF symbol in file: (EOF)
  TOKEN_EOF,
  // Variable's name: ([0-9a-zA-Z]+)
  TOKEN_NAME,
  // Variable declaration start token.
  TOKEN_PERCENT,
  // Rule or-option token: (|)
  TOKEN_PIPE,
  // Rule end token: (;)
  TOKEN_SEMICOLON,
  // Variable char data: (')
  TOKEN_SINGLE_QUOTE,

  // Auxilary token types for tree representation:
  TOKENIZER_TREE,
  PARSER_TREE
} TokenType;

char const *TranslateTokenType(TokenType type);

char *GetSourceText(char const *name);

// Lexer part --------------------------------------------------------------------------------

static const char kSingleQuoteSym = '\'';
static const char kDoubleQuoteSym = '\"';

static const char kNewLineSym = '\n';
static const char kSpaceSym   =  ' ';
static const char kTabSym     = '\t';

// Commentary symbols in source code.
static const char kCommentFirstSym  = '/';
static const char kCommentSecondSym = '*';

static const char  kEscapeSym = '\\';

// Whitespace symbols in source code.
static const char kWhiteSpace[] =
{
  kSpaceSym,
  kNewLineSym,
  kTabSym,
  '\0'
};
// Split symbols to split words in source code.
static const char kSplitSymbols[] =
{
  '(',
  ')',
  ':',
  ';',
  
  kSingleQuoteSym,
  kDoubleQuoteSym,

  kCommentFirstSym,
  kCommentSecondSym,

  '%',
  '|',
  '=',
  '\\',
  '\0'
};

static const size_t kInitSizeNamesArray = 512;

// Types of node for parser.
typedef enum PrsrNdType
{
  NOT_SPECIAL,
   VAR_NAME,
  RULE_NAME,
  RULE_NAME_REFERENCE,
   VAR_NAME_REFERENCE,
} PrsrNdType;

typedef struct Token
{
  // Type of token.
  TokenType  type;
  // Text representation of token.
  char      *txt;
  PrsrNdType parser_type;
} Token;

Token *Tokenizer(char const* const txt, uint64_t *n_tokens);

// AST part ----------------------------------------------------------------------------------

typedef struct
{
  uint64_t n_parsed;
  uint64_t cur_token_idx;
} Context;

// Format of node to print it in graphviz.
#define NODE_FMT                                             \
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
  // bool         rule_name;
  // Type of node in parser generating.
} Node;

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
  // Current node in tree.
  Node       *current;
  // Number of nodes in AST.
  size_t      size;
  const char *data;
} Tree;

// Tree library -----------------------------------------------------------------------------

void Parent(Tree *t);

TokenType GetType(Node *n);

char *GetTxt(Node *n);

Node *GetChild(Node *n, uint64_t idx);

// Constructors -----------------------------------

Node *CreateNodeByType(Tree *t, TokenType type);

Node *CreateNode(Tree *t, Token *token);

Node *NodeCtor();

Tree *TreeCtor();

// Common functions -------------------------------

Node *AddChild(Tree *t, Node *n);

void InsertParent(Tree *t, Node *n);

void AppendTree(Tree *first, Tree *second);

void DebugTree(Tree *t);

// Parser's functions -----------------------------------------------------------------------

void GenerateFiles(Token *sequence, uint64_t n_tokens);

typedef struct NameTable
{
  size_t size;
  Node **names;
} NameTable;

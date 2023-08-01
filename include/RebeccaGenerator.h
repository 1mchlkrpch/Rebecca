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

const char *TranslateTokenType(TokenType type);

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
  // bool         rule_name;
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

char *GetSourceText(const char *name);

void GenerateTrees(Tree *parser_tree, Tree *tokenizer_tree, Token *sequence, uint64_t n_tokens);
// YACC-simmilar tiny ast-builder.
// Tree *GenerateParserAst(Token *sequence, uint64_t n_tokens);
// Translate YACC-similar file to parser file.
// Tree *GenerateParserFile(Token *sequence, uint64_t n_tokens);
void GenerateFiles(Token *sequence, uint64_t n_tokens);

Node *AddChild(Tree *t, Node *n);

TokenType Ttype(Node *n);

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
  Node **names;
} NameTable;

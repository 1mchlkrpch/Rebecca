#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <regex.h>
#include <stdlib.h>

typedef enum
{
	GEN_NOT_SPECIAL,
	   GEN_VAR_NAME,
	  GEN_RULE_NAME,
	  GEN_RULE_NAME_REFERENCE,
	   GEN_VAR_NAME_REFERENCE,
} GEN_PrsrNdType;

static const char *GEN_white_space = " \t\n";
static const char *GEN_splitters = "+-;";
static const int64_t kUndefinedStableWordIdx = -1;

typedef enum {
	default_token,
	PLUS,
	SEMICOLON,
	EOF_T,
	NUMBER_,
	NAME_,
	identifier,
	identifier_1,
	identifier_2,
	additive_expression,
	additive_expression_1,
	additive_expression_2,
	translation_unit,
	translation_unit_1,
	translation_unit_2,
} GEN_TokenType;

typedef struct
{
  // Type of token.
  GEN_TokenType  type;
  // Text representation of token.
  char      *txt;
  GEN_PrsrNdType parser_type;
} GEN_Token;

typedef struct
{
	uint64_t n_parsed;
	uint64_t cur_token_idx;
} GEN_Context;

typedef struct {
	// Text representation of token.
	const char *txt;
	// Length of token.
	size_t      len;
	// Token logic type.
	GEN_TokenType   type;
} GEN_StableWord;

static GEN_StableWord GEN_stable_words[] = {
	{"*", 1, PLUS},
	{";", 1, SEMICOLON},
	{"EOF", 3, EOF_T},
};

static GEN_StableWord GEN_stable_regex_words[] = {
	{"[0-9]+", 6, NUMBER_},
	{"[a-z]+", 6, NAME_},
};

GEN_Token *GEN_Tokenizer(char *name, uint64_t *n_tokens);
const char *GEN_TranslateTokenType(GEN_TokenType type);
#pragma once

#define NODE_FMT                                        \
	"\tn%lu [shape=\"%s\" color=\"%s\" label=\"(%s)\\n%s\"]\n"

#define kTokenMaxLen 256
typedef struct
{
	uint64_t size;
	uint64_t capacity;
	uint64_t element_size;
	void *data;
} GEN_Array;
typedef struct GEN_Node
{
	// Children of particular GEN_node.
	GEN_Array           *children;
	// Token of current cell in AST.
	GEN_Token       *token;
	// Numbered-id for each GEN_node.
	uint64_t     id;
	// Parent GEN_node for current one.
	struct GEN_Node *parent;
	// bool         rule_name;
	// Type of GEN_node in parser generating.
} GEN_Node;
typedef struct
{
	GEN_Node   *root;
	GEN_Node   *current;
	size_t      size;
	const char *data;
} GEN_Tree;
GEN_Node *GEN_NodeCtor();

GEN_Tree *GEN_TreeCtor(GEN_TokenType type);

GEN_Node *GEN_AddChild(GEN_Tree *t, GEN_Node *new_child);

void GEN_InsertParent(GEN_Tree *t, GEN_Node *n);

const char *GEN_CellBordersFormat(GEN_TokenType t);

const char *GEN_CheckIfRuleName(GEN_PrsrNdType type);

GEN_TokenType GEN_Ttype(GEN_Node *n);

void GEN_PrintNode(FILE *f, GEN_Node *n);

void GEN_ConnectNode(FILE *f, GEN_Node *n);

void GEN_DebugTree(GEN_Tree *t);

void GEN_Parent(GEN_Tree *t);

GEN_Node *GEN_GetChild(GEN_Node *n, uint64_t idx);

void GEN_AppendTree(GEN_Tree *first, GEN_Tree *second);

GEN_Node *GEN_CreateNodeByType(GEN_Tree *t, GEN_TokenType type);

GEN_Node *GEN_CreateNode(GEN_Tree *t, GEN_Token *token);

GEN_Array *GEN_ArrayCtor(uint64_t el_sz);
void GEN_ArrayChangeCapacity(GEN_Array *a, uint64_t new_capacity);
void GEN_ArrayAdd(GEN_Array *a, void *new_element);
void GEN_FillEmptyStr(char **dest, const char *src);
void GEN_CompressTree(GEN_Tree *t, GEN_Node *n);

GEN_Context GEN_TryToken(GEN_Tree *t, GEN_Token *sequence, GEN_TokenType expected_type, GEN_Context ctx, uint64_t n_tokens);
GEN_Tree *ParseSequence(GEN_Tree *t, GEN_Token *s, GEN_Context ctx, int64_t n_tokens);
GEN_Context Try_identifier_1(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);
GEN_Context Try_identifier_2(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);
GEN_Context Try_identifier(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);
GEN_Context Try_additive_expression_1(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);
GEN_Context Try_additive_expression_2(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);
GEN_Context Try_additive_expression(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);
GEN_Context Try_translation_unit_1(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);
GEN_Context Try_translation_unit_2(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);
GEN_Context Try_translation_unit(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);

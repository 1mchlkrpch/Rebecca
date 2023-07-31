#pragma once

#include <stdio.h>
#include <stdint.h>
#include <regex.h>
#include <stdlib.h>

#include <include/RebeccaGenerator.h>

static const char *GEN_white_space = " \t\n";
static const char *GEN_splitters = "+-;";
static const int64_t kUndefinedStableWordIdx = -1;

typedef enum {
	PLUS,
	MINUS,
	SEMICOLON,
	NUMBER_,
	IDENTIFIER_,
	EOF_T,
} GEN_TokenType;

typedef struct
{
  // Type of token.
  GEN_TokenType  type;
  // Text representation of token.
  char      *txt;
  // Static value (if exists).
  Value     *value;
  PrsrNdType parser_type;
} GEN_Token;

typedef struct {
	// Text representation of token.
	const char *txt;
	// Length of token.
	size_t      len;
	// Token logic type.
	GEN_TokenType   type;
} GEN_StableWord;

static GEN_StableWord GEN_stable_words[] = {
	{"+", 1, PLUS},
	{"-", 1, MINUS},
	{";", 1, SEMICOLON},
	{"[A-Z]", 5, NUMBER_},
	{"[1-9]", 5, IDENTIFIER_},
	{"EOF", 3, EOF_T},
};

GEN_Token *GEN_Tokenizer(char *name, uint64_t *n_tokens);
const char *GEN_TranslateTokenType(GEN_TokenType type);

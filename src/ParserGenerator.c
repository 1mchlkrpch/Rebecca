#include <stdint.h>
#include <stdio.h>

#include <include/RebeccaGenerator.h>
#include <MchlkrpchLogger/logger.h>

/**
 * @brief Adds to t->current new child 's['idx']'and
 * increments 'current_token_idx'.
 * 
 * @param t     tree to append
 * @param s     sequence to get token
 * @param idx   Index of current token in 's'.
 */
void WorkOutToken(Tree *t, Token *s, uint64_t *idx)
{
	AddChild(t, CreateNode(t, s + *idx));
	++(*idx);
}

void GenerateTrees(Tree *parser_tree, Tree *tokenizer_tree, Token *s, uint64_t n_tokens)
{
	assert(s != NULL && "Null parametr\n");

	msg(D_PARSER_GENERATING, M,
		"Start of generating parser's AST\n");

	size_t token_idx = 0;
	msg(D_PARSER_GENERATING, M,
		"Start parsing\n");

	tab_incr();
	for (; token_idx < n_tokens; ++token_idx) {
		if (s[token_idx].type == TOKEN_PERCENT) {
			msg(D_PARSER_GENERATING, M,
				"Current token in s:(%s)\n",
				s[token_idx].txt);
			/* Skip this token
			And next tokens will be: NAME, EQ, char-data.*/
			++token_idx;
			// Insert Name.
			WorkOutToken(tokenizer_tree, s, &token_idx);
			// Next token will be EQ.
			InsertParent(tokenizer_tree, CreateNode(tokenizer_tree, s + token_idx));
			++token_idx;
			
			if (s[token_idx].type == TOKEN_SINGLE_QUOTE ||
				s[token_idx].type == TOKEN_DOUBLE_QUOTE) {
				++token_idx;
				// Add it's data.
				AddChild(tokenizer_tree, CreateNode(tokenizer_tree, s + token_idx));
				++token_idx;
				// Return to parent node 'EQ'.
				InsertParent(tokenizer_tree, CreateNodeByType(tokenizer_tree, s[token_idx].type));
				Parent(tokenizer_tree);
				// Don't increment 'token_idx' to let it to for-cycle.
			} else if (s[token_idx].type == TOKEN_NAME) {
				// Adds new value equal to another variable.
				WorkOutToken(tokenizer_tree, s, &token_idx);
				// Return to parent 'EQ'
				Parent(tokenizer_tree);
			}

			Parent(tokenizer_tree);
			continue;
		}
		tab_decr();

		// Reading Grammar rule.
		if (s[token_idx].type == TOKEN_NAME) {
			Node *parser_tree_root = parser_tree->current;
			// That is rule so we will add it to parser's tree.
			WorkOutToken(parser_tree, s, &token_idx);
			// Add colon-node.
			WorkOutToken(parser_tree, s, &token_idx);
			while (true) {
				Node *parent = parser_tree->current;
				while (true) {
					if (
						s[token_idx].type == TOKEN_PIPE ||
						s[token_idx].type == TOKEN_SEMICOLON ||
						s[token_idx].type == TOKEN_EOF) {
						break;
					}

					WorkOutToken(parser_tree, s, &token_idx);
				}

				parser_tree->current = parent;
				if (s[token_idx].type == TOKEN_SEMICOLON || s[token_idx].type == TOKEN_EOF) {
					break;
				}
				++token_idx;
			}

			parser_tree->current = parser_tree_root;
			continue;
		}

		++token_idx;
	}
}

int64_t SearchInTable(Node *n, NameTable *table)
{
	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, n->token->txt) == 0) {
			// printf("Already exists\n");
			msg(D_NAMETABLE, M,
				"Already exists\n");

			return cur_el;
		}
	}

	return -1;
}

bool AddName(NameTable *table, Node *n)
{
	assert(table != NULL && "Null parametr\n");
	assert(n     != NULL && "Null parametr\n");

	// Try to find this n in nametable.
	if (SearchInTable(n, table) != -1) {
		return false;
	}
	// Add new name in table.
	// printf("?\n");
	memcpy(table->names + table->size, &n, sizeof(Node*));
	msg(D_NAMETABLE, M,
		"Name was inserted\n");

	++table->size;
	msg(D_NAMETABLE, M,
		"Current size:%ld\n",
		table->size);

	return true;
}

void SearchNamesInBrance(Node *n, NameTable *table)
{
	assert(n     != NULL && "Null parametr\n");
	assert(table != NULL && "Null parametr\n");

	bool is_ref = false;
	if (n->token->type == TOKEN_NAME) {
		if (n->parent != NULL && n->parent->parent != NULL &&
				Ttype(GetChild(n->parent->parent, 0)) == TOKEN_NAME) {

			if (SearchInTable(GetChild(n->parent->parent, 0), table) != -1) {
				n->token->parser_type = VAR_NAME_REFERENCE;
				is_ref = true;
			}
		}

		if (!is_ref) {
			if (AddName(table, n) == true) {
				n->token->parser_type = VAR_NAME;
			} else {
				n->token->parser_type = VAR_NAME_REFERENCE;
			}
		}
	}

	if (n->children != NULL) {
		for (size_t cur_child_idx = 0; cur_child_idx < n->children->size; ++cur_child_idx) {
			Node *cur_child = GetChild(n, cur_child_idx);
			tab_incr();
			SearchNamesInBrance(cur_child, table);
			tab_decr();
			msg(D_NAMETABLE, M,
				"End this child\n");
		}
	}
}

NameTable *ScanTokenizerNames(Tree *t)
{
	assert(t != NULL && "Null parametr\n");
	// Allocate name table for searching familiar names
	NameTable *table = (NameTable*)calloc(1, sizeof(NameTable));
	assert(table != NULL && "Null calloc allocation\n");
	// Allocate array of names.
	table->names = (Node**)calloc(kInitSizeNamesArray, sizeof(Node*));
	assert(table != NULL && "Null calloc allocation\n");

	tab_incr();
	SearchNamesInBrance(t->root, table);
	tab_decr();

	return table;
}

void GenerateConstants(FILE *header_file, Tree *tokenizer_tree, NameTable *table)
{
	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "splitters")   == 0 ||
				strcmp(table->names[cur_el]->token->txt, "white_space") == 0)
		{
			fprintf(header_file, "static const char *GEN_%s = \"%s\";\n",
				table->names[cur_el]->token->txt,
				GetChild(GetChild(table->names[cur_el]->parent, 1), 0)->token->txt);
		}
	}

	fprintf(header_file, "static const int64_t kUndefinedStableWordIdx = -1;\n\n");
}

void GenerateEnumOfTokens(
	FILE *header_file, Tree *tokenizer_tree, NameTable *table, NameTable *parser_table)
{
	fprintf(header_file,
		"typedef enum {\n");
	fprintf(header_file, "\tdefault_token,\n");

	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "splitters") == 0 ||
				strcmp(table->names[cur_el]->token->txt, "start")     == 0 ||
				strcmp(table->names[cur_el]->token->txt, "white_space") == 0) {
			continue;
		}

		fprintf(header_file, "\t%s,\n", table->names[cur_el]->token->txt);
	}
	for (size_t cur_el = 0; cur_el < parser_table->size; ++cur_el) {
		if (strcmp(parser_table->names[cur_el]->token->txt, "splitters") == 0 ||
			strcmp(parser_table->names[cur_el]->token->txt, "start")     == 0 ||
			strcmp(parser_table->names[cur_el]->token->txt, "white_space") == 0) {
			continue;
		}

		// printf("generate tokens\n");
		Node *in_parser_tree = parser_table->names[cur_el];
		fprintf(header_file, "\t%s,\n", in_parser_tree->token->txt);
		for (size_t cur_child = 0; cur_child < GetChild(in_parser_tree, 0)->children->size; ++cur_child) {
			fprintf(header_file, "\t%s_%ld,\n", in_parser_tree->token->txt, cur_child + 1);
		}
	}

	fprintf(header_file,
		"} GEN_TokenType;\n\n");

	fprintf(header_file,
		"typedef struct\n"
		"{\n"
		"  // Type of token.\n"
		"  GEN_TokenType  type;\n"
		"  // Text representation of token.\n"
		"  char      *txt;\n"
		"  GEN_PrsrNdType parser_type;\n"
		"} GEN_Token;\n\n");
}

void GenerateArrayOfKeywords(FILE *header_file, Tree *tokenizer_tree, NameTable *table)
{
	fprintf(header_file,
		"typedef struct {\n"
		"\t// Text representation of token.\n"
		"\tconst char *txt;\n"
		"\t// Length of token.\n"
		"\tsize_t      len;\n"
		"\t// Token logic type.\n"
		"\tGEN_TokenType   type;\n"
		"} GEN_StableWord;\n\n");

	fprintf(header_file,
		"static GEN_StableWord GEN_stable_words[] = {\n");

	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "splitters") == 0 ||
			strcmp(table->names[cur_el]->token->txt, "start")     == 0 ||
			strcmp(table->names[cur_el]->token->txt, "white_space") == 0) {
			continue;
		}

		char *label = table->names[cur_el]->token->txt;
		if (label[strlen(label) - 1] == '_') {
			continue;
		}

		fprintf(header_file, "\t{\"%s\", %ld, %s},\n",
			GetChild(GetChild(table->names[cur_el]->parent, 1), 0)->token->txt,
			strlen(GetChild(GetChild(table->names[cur_el]->parent, 1), 0)->token->txt),
			table->names[cur_el]->token->txt);
	}

	fprintf(header_file,
		"};\n\n");

	fprintf(header_file,
		"static GEN_StableWord GEN_stable_regex_words[] = {\n");

	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "splitters") == 0 ||
			strcmp(table->names[cur_el]->token->txt, "start")     == 0 ||
			strcmp(table->names[cur_el]->token->txt, "white_space") == 0) {
			continue;
		}

		char *label = table->names[cur_el]->token->txt;
		if (label[strlen(label) - 1] == '_') {
			fprintf(header_file, "\t{\"%s\", %ld, %s},\n",
				GetChild(GetChild(table->names[cur_el]->parent, 1), 0)->token->txt,
				strlen(GetChild(GetChild(table->names[cur_el]->parent, 1), 0)->token->txt),
				table->names[cur_el]->token->txt);
		}

	}

	fprintf(header_file,
		"};\n\n");
}

void GenerateCommonCommands(FILE *c_file, Tree *tokenizer_tree, NameTable *table)
{
	fprintf(c_file,
		"int64_t GEN_IdentifyToken(char *cur_word)\n"
		"{\n"
		"\tassert(cur_word != NULL && \"nullptr param\");\n"
		"\tfor (size_t idx = 0; idx < sizeof(GEN_stable_words) / sizeof(GEN_StableWord); ++idx) {\n"
		"\t\tif (strcmp(GEN_stable_words[idx].txt, cur_word) == 0) {\n"
		"\t\t\treturn idx;\n"
		"\t\t}\n"

		"\t}\n\n"
		"\treturn kUndefinedStableWordIdx;\n"
		"}\n\n"

		"bool GEN_CanBeAppended(char *cur_word, char *cursor, uint64_t cur_token_len)\n"
		"{\n"
		"\tchar *copy = (char *)calloc(kTokenMaxLen, sizeof(char));\n"
		"\tassert(copy != NULL && \"Null calloc allocation\");\n"
		"\tstrcpy(copy, cur_word);\n"
		"\tcopy[cur_token_len] = *cursor;\n"
		"\tif (GEN_IdentifyToken(copy) == kUndefinedStableWordIdx) {\n"
		"\t\treturn false;\n"
		"\t} else {\n"
		"\t\treturn true;\n"
		"\t}\n"
		"}\n\n"

		"GEN_Token GEN_ConstructToken(uint64_t idx)\n"
		"{\n"
		"\tGEN_Token token = {0};\n"
		"\ttoken.txt = (char *)calloc(kTokenMaxLen, sizeof(char));\n"
		"\tassert(token.txt != NULL && \"Null calloc allocation\");\n"
		"\ttoken.type = GEN_stable_words[idx].type;\n"
		"\tstrcpy(token.txt, GEN_stable_words[idx].txt);\n"
		"\treturn token;\n"
		"}\n\n"

		"GEN_Token GEN_FillToken(const char *txt)\n"
		"{\n"
		"\tGEN_Token token = {0};\n"
		"\ttoken.txt = (char *)calloc(kTokenMaxLen, sizeof(char));\n"
		"\tassert(token.txt != NULL && \"Null calloc allocation\");\n"
		"\ttoken.type = 0;\n"
		"\tstrcpy(token.txt, txt);\n"
		"\treturn token;\n"
		"}\n\n"
		
		"void GEN_PushToken(GEN_Token *token, GEN_Token *sequence, uint64_t *sequence_size)\n"
		"{\n"
		"\tsequence[*sequence_size] = *token;\n"
		"\t++(*sequence_size);\n"
		"}\n\n"
		
		"void GEN_TryPushToken(\n"
		"\tuint64_t *cur_token_len, char *cur_word, GEN_Token *sequence, uint64_t *sequence_size)\n"
		"{\n"
		"\tif (*cur_token_len > 0) {\n"
		"\t\tuint64_t idx = GEN_IdentifyToken(cur_word);\n"
		"\t\tGEN_Token token = {0};\n"
		"\t\tif (idx != kUndefinedStableWordIdx) {\n"
		"\t\t\ttoken = GEN_ConstructToken(idx);\n"
		"\t\t} else {\n"
		"\t\t\tbool is_regex = false;\n"
		"\t\t\tregex_t regex;\n"
		"\t\t\tfor (size_t idx = 0; idx < sizeof(GEN_stable_regex_words) / sizeof(GEN_StableWord); ++idx) {\n"
		"\t\t\t\tint reti = regcomp(&regex, GEN_stable_regex_words[idx].txt, REG_EXTENDED | REG_NOSUB);\n"
		"\t\t\t\tif (reti == 0) {\n"
		"\t\t\t\t\tint res = regexec(&regex, cur_word, 0, NULL, 0);\n"
		"\t\t\t\t\tif (res == 0) {\n"
		"\t\t\t\t\t\ttoken.txt = (char *)calloc(kTokenMaxLen, sizeof(char));\n"
		"\t\t\t\t\t\tassert(token.txt != NULL && \"Null calloc allocation\");\n"
		"\t\t\t\t\t\ttoken.type = GEN_stable_regex_words[idx].type;\n"
		"\t\t\t\t\t\tstrcpy(token.txt, cur_word);\n"
		"\t\t\t\t\t\tis_regex = true;\n"
		"\t\t\t\t\t}\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n"
		"\t\t\tif (!is_regex) {\n"
		"\t\t\t	token = GEN_FillToken(cur_word);\n"
		"\t\t\t}\n"
		"\t\t}\n"
		"\t\tGEN_PushToken(&token, sequence, sequence_size);\n"
		"\t\t*cur_token_len = 0;\n"
		"\t\tmemset(cur_word, \'\\0\', kTokenMaxLen);\n"
		"\t}\n"
		"}\n\n"

		"void GEN_PushEof(uint64_t *cur_token_len, char *cur_word, GEN_Token *sequence, uint64_t *n_tokens)\n"
		"{\n"
		"\t*cur_token_len = 3;\n"
		"\tmemset(cur_word, \'\\0\', kTokenMaxLen);\n"
		"\tstrcpy(cur_word, \"EOF\");\n"
		"\tGEN_TryPushToken(cur_token_len, cur_word, sequence, n_tokens);\n"
		"}\n\n"
	);
}

void GenerateSplittersCommands(FILE *c_file, Tree *tokenizer_tree, NameTable *table)
{
	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "splitters") == 0) {
			fprintf(c_file,
				"static inline __attribute__((always_inline))\n"
				"bool GEN_IsSplit(const char c)\n"
				"{ return strchr(GEN_splitters, c) != NULL; }\n\n");
			break;
		}
	}

	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "white_space") == 0) {
			fprintf(c_file,
				"static inline __attribute__((always_inline))\n"
				"bool GEN_IsWhiteSpace(const char c)\n"
				"{ return strchr(GEN_white_space, c) != NULL; }\n\n");
			break;
		}
	}
}

void GenerateTokenizerCmd(FILE *c_file, Tree *tokenizer_tree, NameTable *table)
{
	fprintf(c_file,
		"GEN_Token *GEN_Tokenizer(char *source_text, uint64_t *n_tokens)\n"
		"{\n"
		"\tassert(source_text != NULL && \"nullptr param\");\n"
		"\tassert(n_tokens    != NULL && \"nullptr param\");\n\n"

		"\tchar *cursor = source_text;\n"
		"\tGEN_Token *sequence = (GEN_Token *)calloc(kInitSequenceSize, sizeof(GEN_Token));\n"
		"\tassert(sequence != NULL && \"Null calloc allocation\");\n"
		
		"\tuint64_t cur_token_len = 0;\n"
		"\tchar cur_word[kTokenMaxLen] = \"\";\n"
		
		"\twhile (*cursor != EOF) {\n"
		"\t\tif (GEN_IsWhiteSpace(*cursor)) {\n"
		"\t\t\twhile (GEN_IsWhiteSpace(*cursor) && *cursor != EOF) {\n"
		"\t\t\t\t++cursor;\n"
		"\t\t\t}\n"
		"\t\t\tGEN_TryPushToken(&cur_token_len, cur_word, sequence, n_tokens);\n"
		"\t\t\tcontinue;\n"
		"\t\t}\n"
		"\t\tif (GEN_IsSplit(*cursor)) {\n"
		"\t\t\tGEN_TryPushToken(&cur_token_len, cur_word, sequence, n_tokens);\n"
		"\t\t\twhile (GEN_IsSplit(*cursor)  && *cursor != EOF) {\n"
		"\t\t\t\tcur_word[cur_token_len] = *cursor;\n"
		"\t\t\t\t++cur_token_len;\n"
		"\t\t\t\t++cursor;\n"
		"\t\t\t\tif (GEN_CanBeAppended(cur_word, cursor, cur_token_len)) {\n"
		"\t\t\t\t\tcontinue;\n"
		"\t\t\t\t} else {\n"
		"\t\t\t\t\tbreak;\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n"
		"\t\t\tGEN_TryPushToken(&cur_token_len, cur_word, sequence, n_tokens);\n"
		"\t\t\tcontinue;\n"
		"\t\t}\n"
		"\t\tcur_word[cur_token_len] = *cursor;\n"
		"\t\t++cur_token_len;\n"
		"\t\t++cursor;\n"
		"\t}\n"
		
		"\tGEN_TryPushToken(&cur_token_len, cur_word, sequence, n_tokens);\n"
		"\tGEN_PushEof(&cur_token_len, cur_word, sequence, n_tokens);\n"
		"\tfree(source_text);\n\n"
		
		"\treturn sequence;\n"
		"}\n\n");
}

void GenerateTranslationCommand(FILE *c_file, Tree *tokenizer_tree, NameTable *table, NameTable *parser_table)
{
	fprintf(c_file,
		"const char *GEN_TranslateTokenType(GEN_TokenType type)\n"
		"{\n"
		"\tswitch (type) {\n");

	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "splitters")   == 0 ||
			strcmp(table->names[cur_el]->token->txt, "start")       == 0 ||
			strcmp(table->names[cur_el]->token->txt, "white_space") == 0) {
			continue;
		}

		fprintf(c_file,
			"\t\tcase %s: { return \"%s\"; }\n",
			table->names[cur_el]->token->txt,
			table->names[cur_el]->token->txt);
	}

	for (size_t cur_el = 0; cur_el < parser_table->size; ++cur_el) {
		if (strcmp(parser_table->names[cur_el]->token->txt, "splitters") == 0 ||
			strcmp(parser_table->names[cur_el]->token->txt, "start")     == 0 ||
			strcmp(parser_table->names[cur_el]->token->txt, "white_space") == 0) {
			continue;
		}

		// printf("generate tokens\n");
		Node *in_parser_tree = parser_table->names[cur_el];
		// fprintf(header_file, "\t%s,\n", in_parser_tree->token->txt);
		fprintf(c_file,
			"\t\tcase %s: { return \"%s\"; }\n",
			parser_table->names[cur_el]->token->txt,
			parser_table->names[cur_el]->token->txt);
		for (size_t cur_child = 0; cur_child < GetChild(in_parser_tree, 0)->children->size; ++cur_child) {
			fprintf(c_file,
				"\t\tcase %s_%lu: { return \"%s_%lu\"; }\n",
				in_parser_tree->token->txt, cur_child + 1,
				in_parser_tree->token->txt, cur_child + 1
				);
		}
	}

	fprintf(c_file,
		"\t\tdefault: {\n"
		"\t\t\treturn \"default_token\";\n"
		"\t\t}\n"
		"\t}\n"
		"\treturn \"default_token\";\n"
		"}\n\n"
		);
}

void GenerateTokenizerFile(
	Tree *tokenizer_tree, NameTable *table, FILE *header_file, NameTable *parser_table)
{
	assert(tokenizer_tree != NULL && "Null param");
	assert(table          != NULL && "Null param");

	// FILE *header_file = fopen("../src/Tokenizer_GEN.h", "w");
	FILE *c_file      = fopen("../out/Tokenizer_GEN.c", "w");

	fprintf(header_file,
		"#pragma once\n\n"
		"#include <assert.h>\n"
		"#include <stdbool.h>\n"
		"#include <stdio.h>\n"
		"#include <string.h>\n"
		"#include <stdint.h>\n"
		"#include <regex.h>\n"
		"#include <stdlib.h>\n\n");

	fprintf(header_file,
		"static const uint64_t kMaxScopeDepth = 256;\n"
		"static const uint64_t kInitSequenceSize = 1024;\n\n");
	
	fprintf(header_file,
		"typedef enum\n"
		"{\n"
		"\tGEN_NOT_SPECIAL,\n"
		"\t   GEN_VAR_NAME,\n"
		"\t  GEN_RULE_NAME,\n"
		"\t  GEN_RULE_NAME_REFERENCE,\n"
		"\t   GEN_VAR_NAME_REFERENCE,\n"
		"} GEN_PrsrNdType;\n\n"
		);

	GenerateConstants(header_file, tokenizer_tree, table);
	GenerateEnumOfTokens(header_file, tokenizer_tree, table, parser_table);

	fprintf(header_file,
		"typedef struct\n"
		"{\n"
		"\tuint64_t n_parsed;\n"
		"\tuint64_t cur_token_idx;\n"
		"} GEN_Context;\n\n"
		);

	GenerateArrayOfKeywords(header_file, tokenizer_tree, table);
	fprintf(header_file,
		"GEN_Token *GEN_Tokenizer(char *name, uint64_t *n_tokens);\n");

	fprintf(header_file,
		"const char *GEN_TranslateTokenType(GEN_TokenType type);\n");

	// fclose(header_file);
	// C-file:
	fprintf(c_file,
		"#include <Tokenizer_GEN.h>\n\n");
	GenerateSplittersCommands(c_file, tokenizer_tree, table);
	GenerateCommonCommands(c_file, tokenizer_tree, table);

	GenerateTranslationCommand(c_file, tokenizer_tree, table, parser_table);

	GenerateTokenizerCmd(c_file, tokenizer_tree, table);

	fclose(c_file);
}

// Generation parser file. -------------------------------------------------------------
void WritePrefixOfParserFunction(FILE *header_file, FILE *c_file, const char *name_of_rule)
{
	assert(header_file  != NULL && "Null parametr\n");
	assert(name_of_rule != NULL && "Null parametr\n");

	fprintf(header_file,
		"GEN_Context Try_%s(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);\n"
		, name_of_rule);

	fprintf(c_file, "GEN_Context Try_%s(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)\n{\n"
		"\tGEN_Context try_ctx = ctx;\n"
		"\tGEN_Tree %s_tree = {0};\n\n", name_of_rule, name_of_rule);

	fprintf(c_file,
		"\tGEN_AddChild(&%s_tree, GEN_CreateNodeByType(&%s_tree, %s));\n"
		"\tGEN_Context new_ctx = {0};\n\n", name_of_rule, name_of_rule, name_of_rule);
}

void WriteReferenceToOtherRules(FILE *header_file, char *tabs, uint64_t cur_child, uint64_t *n_tabs, char *name_of_rule)
{
	tabs[(*n_tabs)++] = '\t';

	fprintf(header_file,
		"%snew_ctx = Try_%s_%lu(&%s_tree, sequence, try_ctx, n_tokens);\n",
		tabs, name_of_rule, cur_child + 1, name_of_rule);

	fprintf(header_file,
		"%sif (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {\n",
		tabs);
}

void WriteObviousCommands(FILE *header_file, FILE *c_file, NameTable *table)
{
	fprintf(header_file,
		"GEN_Context GEN_TryToken(GEN_Tree *t, GEN_Token *sequence, GEN_TokenType expected_type, GEN_Context ctx, uint64_t n_tokens);\n");

	fprintf(c_file,
		"GEN_Context GEN_TryToken(GEN_Tree *t, GEN_Token *sequence, GEN_TokenType expected_type, GEN_Context ctx, uint64_t n_tokens)\n"
		"{\n"
		"\tmsg(D_PARSER_WORK, M, \"TryToken start t(%%s|idx:%%lu)\\n\", sequence[ctx.cur_token_idx].txt, ctx.cur_token_idx);\n"
		"\tGEN_Tree try_token_tree = {0};\n"
		"\tGEN_AddChild(&try_token_tree, GEN_CreateNode(&try_token_tree, sequence + n_tokens - 1));\n"
		"\tif (sequence[ctx.cur_token_idx].type == expected_type) {\n"
		"\t\tGEN_AddChild(&try_token_tree, GEN_CreateNode(&try_token_tree, sequence + ctx.cur_token_idx));\n"
		"\t\t++ctx.n_parsed;\n"
		"\t\t++ctx.cur_token_idx;\n"
		"\t} else { return ctx; }\n\n"
		"\tGEN_Parent(&try_token_tree);\n"
		"\tGEN_AppendTree(t, &try_token_tree);\n"
		"\tmsg(D_PARSER_WORK, M, \"END of TryToken\\n\");\n"
		"\treturn ctx;\n"
		"}\n\n");

	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "start") == 0) {
			fprintf(c_file,
				"GEN_Tree *ParseSequence(GEN_Tree *t, GEN_Token *s, GEN_Context ctx, int64_t n_tokens) {\n"
				"\tTry_%s(t, s, ctx, n_tokens);\n"
				"\treturn t;\n"
				"}\n\n",
				GetChild(GetChild(table->names[cur_el]->parent, 1), 0)->token->txt);
			fprintf(header_file,
				"GEN_Tree *ParseSequence(GEN_Tree *t, GEN_Token *s, GEN_Context ctx, int64_t n_tokens);\n");
			break;
		}

	}
}

void GenerateCommand(FILE *header_file, FILE *c_file, Node *n)
{
	assert(header_file != NULL && "Null parametr\n");
	assert(n           != NULL && "Null parametr\n");
	assert(n->children != NULL && "Null parametr\n");

	/* Name of current name
	Of rule to use it as part of names of variables
	or names of parser functions.*/
	char *name_of_rule = n->token->txt;

	/* To generate more beautiful code
	Function use tabs and '\n'-symbols*/
	char tabs[kTokenMaxLen] = "";
	uint64_t n_tabs = 0;

	Node *fork = GetChild(n, 0);

	for (uint64_t cur_child = 0; cur_child < fork->children->size; ++cur_child) {
		fprintf(header_file,
			"GEN_Context Try_%s_%ld(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);\n",
			name_of_rule, cur_child + 1);
		// Firstly we will write all the options of this rule to the parser file.
		fprintf(c_file,
			"GEN_Context Try_%s_%ld(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)\n{\n",
			name_of_rule, cur_child + 1);

		tabs[n_tabs++] = '\t';

		fprintf(c_file,
			"%smsg(D_PARSER_WORK, M, \"%s\\n\");\n",
			tabs, name_of_rule);

		fprintf(c_file,
			"%sGEN_Context try_ctx = ctx;\n"
			"%sGEN_Tree %s_%ld_tree = {0};\n",
			tabs, tabs, name_of_rule, cur_child + 1);

		fprintf(c_file,
			"%sGEN_AddChild(&%s_%lu_tree, GEN_CreateNodeByType(&%s_%lu_tree, %s_%lu));\n"
			"%sGEN_Context new_ctx = {0};\n\n",
			tabs, name_of_rule, cur_child + 1,
			name_of_rule, cur_child + 1,
			name_of_rule, cur_child + 1,
			tabs);

		Node *chain = GetChild(fork, cur_child);
		tab_incr();
		while (chain->children != NULL) {
			WriteChain(c_file, chain, tabs, cur_child, name_of_rule);
			chain = GetChild(chain, 0);
		}
		/* Write last chain in sequence in particular
		line in rule.*/
		WriteChain(c_file, chain, tabs, cur_child, name_of_rule);
		fprintf(c_file,
			"\tGEN_AppendTree(t, &%s_%lu_tree);\n",
			name_of_rule, cur_child + 1);

		tab_decr();
		msg(D_FILE_PRINT, M,
					"end of line\n");

		fprintf(c_file,
			"%smsg(D_PARSER_WORK, M, \"EXACTLY Try_%s_%lu\\n\");\n",
			tabs, name_of_rule, cur_child + 1);

		fprintf(c_file,
			"%sGEN_InsertParent(t, GEN_CreateNode(t, sequence + n_tokens - 1));\n",
			tabs);

		--n_tabs;
		tabs[n_tabs] = '\0';

		fprintf(c_file, "\treturn try_ctx;\n");
		fprintf(c_file, "}\n\n");
	}

	/* Secondly print prefix of parser
	function that contains local GEN_context - if that function
	can be applied to the current place in sequence of token
	this GEN_context will commit it and contain local progress
	in parsing sequence.*/
	WritePrefixOfParserFunction(header_file, c_file, name_of_rule);

	msg(D_PARSER_GENERATING, M,
		"Not tabs:\"%s\"(%lu)\n",
		tabs, n_tabs);

	msg(D_PARSER_GENERATING, M,
		"Current rule has (%lu) options\n",
		fork->children->size);

	for (uint64_t cur_child = 0; cur_child < fork->children->size; ++cur_child) {
		WriteReferenceToOtherRules(c_file, tabs, cur_child, &n_tabs, name_of_rule);
	}

	/* If all rules can't be applied to the sequence that was wrong rule
	to parse current place in token sequence so we return original contest.*/
	fprintf(c_file, "%s\ttab_decr();\n", tabs);
	fprintf(c_file, "%s\treturn ctx;\n", tabs);

	// Write all close-braces for all ifs.
	for (uint64_t cur_child = 0; cur_child < fork->children->size; ++cur_child) {
		fprintf(c_file,
			"%s}\n",
			tabs);

		--n_tabs;
		tabs[n_tabs] = '\0';
	}

	/* If one rule-option can be applied to the token sequence
	we can add it's tree that was build by this option to the parent tree.*/
	fprintf(c_file,
		"\n\tGEN_AppendTree(t, &%s_tree);\n"
		"\treturn new_ctx;\n", name_of_rule);

	fprintf(c_file, "}\n\n");
}

void WriteChain(FILE *c_file, Node *chain, char *tabs, uint64_t cur_child, char *name_of_rule)
{
	msg(D_FILE_PRINT, M,
		"Current node in line:%s\n", chain->token->txt);

	fprintf(c_file,
		"%stab_incr();\n"
		"%smsg(D_PARSER_WORK, M, \"new chain in option\\n\");\n"
		"%stab_incr();\n",
		tabs, tabs, tabs);

	if (chain->token->parser_type == NOT_SPECIAL) {
		fprintf(c_file,
			"%snew_ctx = GEN_TryToken(&%s_%lu_tree, sequence, %s, try_ctx, n_tokens);\n",
			tabs, name_of_rule, cur_child + 1, TranslateTokenType(chain->token->type));

	} else if (chain->token->parser_type == RULE_NAME_REFERENCE) {
		fprintf(c_file,
			"%snew_ctx = Try_%s(&%s_%lu_tree, sequence, try_ctx, n_tokens);\n",
			tabs, chain->token->txt, name_of_rule, cur_child + 1);
	} else if (chain->token->parser_type == VAR_NAME_REFERENCE) {
		fprintf(c_file,
			"%snew_ctx = GEN_TryToken(&%s_%lu_tree, sequence, %s, try_ctx, n_tokens);\n",
			tabs, name_of_rule, cur_child + 1, chain->token->txt);
	}

	fprintf(c_file,
		"%stab_decr();\n",
		tabs);

	fprintf(c_file,
		"%sif (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {\n"
		"%s\tmsg(D_PARSER_WORK, M, \"NOT NEEDED OPTION: Try_%s_%lu\\n\");\n"
		"%s\ttab_decr();\n"
		"%s\treturn ctx;\n"
		"%s}\n"
		"%stry_ctx = new_ctx;\n",
		tabs, tabs, name_of_rule, cur_child + 1, tabs, tabs, tabs, tabs);

	fprintf(c_file,
		"%smsg(D_PARSER_WORK, M, \"END chain in option\\n\");\n"
		"%stab_decr();\n\n",
		tabs, tabs);
}

void GenerateCommands(FILE *header_file, FILE *c_file, Node *n)
{
	assert(header_file != NULL && "Null parametr\n");
	assert(n           != NULL && "Null parametr\n");

	if (n->token->parser_type == RULE_NAME) {
		// Check if it's rule root node.
		msg(D_FILE_PRINT, M,
			"Found rule-node:\"%s\"\n",
			n->token->txt);
		/* If that's rule name we will
		generate function text and print it
		to the parser file.*/
		tab_incr();
		GenerateCommand(header_file, c_file, n);
		tab_decr();
	} else {
		if (n->children != NULL) {
			/*If that node is not a rule-root node, maybe it
			contains rule-node in it's children so we will check
			nodes one by one.*/
			for (uint64_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
				msg(D_FILE_PRINT, M,
					"This node NOT rule:\"%s\"\n",
					n->token->txt);
				msg(D_FILE_PRINT, M,
					"Start to check childs nodes\n");
				tab_incr();
				GenerateCommands(header_file, c_file, GetChild(n, cur_child));
				tab_decr();
				msg(D_FILE_PRINT, M,
					"End of parsing children\n");
			}
		}
	}
}

//////////////////////////////////////////////////
void SearchNamesInBranceParser(Node *n, NameTable *table, NameTable *parser_table)
{
	assert(n     != NULL && "Null parametr\n");
	assert(table != NULL && "Null parametr\n");

	bool is_ref = false;
	if (n->token->type == TOKEN_NAME) {
		// if (n->parent != NULL && n->parent->parent != NULL &&
		// 		Ttype(GetChild(n->parent->parent, 0)) == TOKEN_NAME) {

		// 	if (SearchInTable(GetChild(n->parent->parent, 0), table) != -1) {
		// 		n->token->parser_type = VAR_NAME_REFERENCE;
		// 		is_ref = true;
		// 	}
		// }
		if (SearchInTable(n, table) != -1) {
			n->token->parser_type = VAR_NAME_REFERENCE;
			is_ref = true;
		}

		if (!is_ref) {
			if (n->token->type == TOKEN_NAME &&
					n->children != NULL &&
					GetChild(n,0)->token->type == TOKEN_COLON) {
				n->token->parser_type = RULE_NAME;
			} else {
				n->token->parser_type = RULE_NAME_REFERENCE;
			}
		}

		// if (SearchInTable(n, parser_table) != -1) {
		// 	n->token->parser_type = RULE_NAME_REFERENCE;
		// 	is_ref = true;
		// }

		// if (!is_ref) {
		// 	AddName(parser_table, n);
		// 	n->token->parser_type = RULE_NAME;
		// 	// if (n->children != NULL && GetChild(n, 0)->token->type == TOKEN_SEMICOLON) {
		// 	// } else {
		// 	// 	n->token->parser_type = RULE_NAME_REFERENCE;
		// 	// }
		// 	// if (AddName(parser_table, n) == true) {
		// 	// 	n->token->parser_type = VAR_NAME;
		// 	// } else {
		// 	// 	n->token->parser_type = VAR_NAME_REFERENCE;
		// 	// }
		// }
	}

	if (n->children != NULL) {
		for (size_t cur_child_idx = 0; cur_child_idx < n->children->size; ++cur_child_idx) {
			Node *cur_child = GetChild(n, cur_child_idx);
			tab_incr();
			SearchNamesInBranceParser(cur_child, table, parser_table);
			tab_decr();
			msg(D_NAMETABLE, M,
				"End this child\n");
		}
	}
}

void FindAllRuleNames(Node *n, NameTable *parser_table)
{
	if (n->token->type == TOKEN_NAME &&
			n->children != NULL &&
			GetChild(n,0)->token->type == TOKEN_COLON) {
		printf("added node:(%s)\n", n->token->txt);
		AddName(parser_table, n);
	}

	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			FindAllRuleNames(GetChild(n, cur_child), parser_table);
		}
	}
}

NameTable *ScanParserNames(Tree *parser_tree, NameTable *table)
{
	assert(parser_tree != NULL && "Null parametr\n");
	// Allocate name table for searching familiar names
	NameTable *parser_table = (NameTable*)calloc(1, sizeof(NameTable));
	assert(parser_table != NULL && "Null calloc allocation\n");
	// Allocate array of names.
	parser_table->names = (Node**)calloc(kInitSizeNamesArray, sizeof(Node*));
	assert(parser_table != NULL && "Null calloc allocation\n");

	FindAllRuleNames(parser_tree->root, parser_table);

	tab_incr();
	SearchNamesInBranceParser(parser_tree->root, table, parser_table);
	tab_decr();

	return parser_table;
}

void GenerateParserFile(Tree *t, Token *s, uint64_t n_tokens, NameTable *table, FILE *header_file)
{
	// FILE *header_file = fopen("../src/Parser_GEN.h", "w");
	FILE *c_file = fopen("../out/Parser_GEN.c", "w");

	fprintf(c_file,
		"#include <../MchlkrpchLogger/logger.h>\n\n"
		"#include <Tokenizer_GEN.h>\n\n"
		);

	WriteObviousCommands(header_file, c_file, table);

	tab_incr();
	GenerateCommands(header_file, c_file, t->root);
	tab_decr();

	fclose(c_file);
}

void GenerateTreeFile(FILE *header_file)
{
	FILE *c_file = fopen("../out/Tree_GEN.c", "w");

	fprintf(header_file,
		"#pragma once\n\n"
		"#define NODE_FMT                                        \\\n"
  	"\t\"\\tn%%lu [shape=\\\"%%s\\\" color=\\\"%%s\\\" label=\\\"(%%s)\\\\n%%s\\\"]\\n\"\n\n"
  	"#define kTokenMaxLen 256\n"
		"typedef struct\n"
		"{\n"
		"	uint64_t size;\n"
		"	uint64_t capacity;\n"
		"	uint64_t element_size;\n"
		"	void *data;\n"
		"} GEN_Array;\n\n"

		"typedef struct GEN_Node\n"
		"{\n"
		"\t// Children of particular GEN_node.\n"
		"\tGEN_Array           *children;\n"
		"\t// Token of current cell in AST.\n"
		"\tGEN_Token       *token;\n"
		"\t// Numbered-id for each GEN_node.\n"
		"\tuint64_t     id;\n"
		"\t// Parent GEN_node for current one.\n"
		"\tstruct GEN_Node *parent;\n"
		"\t// bool         rule_name;\n"
		"\t// Type of GEN_node in parser generating.\n"
		"} GEN_Node;\n"
		"typedef struct\n"
		"{\n"
		"\tGEN_Node   *root;\n"
		"\tGEN_Node   *current;\n"
		"\tsize_t      size;\n"
		"\tconst char *data;\n"
		"} GEN_Tree;\n"
		"GEN_Node *GEN_NodeCtor();\n\n"
		"GEN_Tree *GEN_TreeCtor(GEN_TokenType type);\n\n"
		"GEN_Node *GEN_AddChild(GEN_Tree *t, GEN_Node *new_child);\n\n"
		"void GEN_InsertParent(GEN_Tree *t, GEN_Node *n);\n\n"
		"const char *GEN_CellBordersFormat(GEN_TokenType t);\n\n"
		"const char *GEN_CheckIfRuleName(GEN_PrsrNdType type);\n\n"
		"GEN_TokenType GEN_Ttype(GEN_Node *n);\n\n"
		"void GEN_PrintNode(FILE *f, GEN_Node *n);\n\n"
		"void GEN_ConnectNode(FILE *f, GEN_Node *n);\n\n"
		"void GEN_DebugTree(GEN_Tree *t);\n\n"
		"void GEN_Parent(GEN_Tree *t);\n\n"
		"GEN_Node *GEN_GetChild(GEN_Node *n, uint64_t idx);\n\n"
		"void GEN_AppendTree(GEN_Tree *first, GEN_Tree *second);\n\n"
		"GEN_Node *GEN_CreateNodeByType(GEN_Tree *t, GEN_TokenType type);\n\n"
		"GEN_Node *GEN_CreateNode(GEN_Tree *t, GEN_Token *token);\n\n"

		"GEN_Array *GEN_ArrayCtor(uint64_t el_sz);\n"
		"void GEN_ArrayChangeCapacity(GEN_Array *a, uint64_t new_capacity);\n"
		"void GEN_ArrayAdd(GEN_Array *a, void *new_element);\n"
		"void GEN_FillEmptyStr(char **dest, const char *src);\n"
		);

	fprintf(header_file,
		"void GEN_CompressTree(GEN_Tree *t, GEN_Node *n);\n\n");

	fprintf(c_file,
		"#include <Tokenizer_GEN.h>\n"
		"#include <assert.h>\n"
		"#include <MchlkrpchLogger/logger.h>\n\n");

	fprintf(c_file,
		"GEN_Node *GEN_NodeCtor()\n"
		"{\n"
		"\tGEN_Node *n = (GEN_Node *)calloc(1, sizeof(GEN_Node));\n"
		"\tn->token = (GEN_Token *)calloc(1, sizeof(GEN_Token));\n"
		"\treturn n;\n"
		"}\n\n"

		"GEN_Tree *GEN_TreeCtor(GEN_TokenType type)\n"
		"{\n"
		"\tGEN_Tree *new_tree = (GEN_Tree *)calloc(1, sizeof(GEN_Tree));\n"
		"\tassert(new_tree != NULL && \"Null calloc allocation\");\n"
		"\tGEN_AddChild(new_tree, GEN_CreateNodeByType(new_tree, type));\n"
		"\treturn new_tree;\n"
		"}\n\n"

		"GEN_Node *GEN_AddChild(GEN_Tree *t, GEN_Node *new_child)\n"
		"{\n"
		"\tif (t->current == NULL) {\n"
		"\t\tt->root    = new_child;\n"
		"\t\tt->current = t->root;\n"
		"\t\t} else {\n"
		"\t\tif (t->current->children == NULL) {\n"
		"\t\t	t->current->children = GEN_ArrayCtor(sizeof(GEN_Node*));\n"
		"\t\t}\n"
		"\t\tGEN_ArrayAdd(t->current->children, new_child);\n"
		"\t\tGEN_GetChild(t->current, t->current->children->size - 1)->parent = (struct GEN_Node *)t->current;\n"
		"\t\tt->current = new_child;\n"
		"\t}\n"
		"\treturn t->current;\n"
		"}\n\n"

		"void GEN_InsertParent(GEN_Tree *t, GEN_Node *n)\n"
		"{\n"
		"\tGEN_Node *old_current = t->current;\n"
		"\tif (t->current->parent != NULL) {\n"
		"\t\tGEN_Node *parent = t->current->parent;\n"
		"\t\tmemcpy((GEN_Node **)parent->children->data + parent->children->size - 1, &n, sizeof(GEN_Node*));\n"
		"\t\tn->parent = parent;\n"
		"\t}\n"
		"\tt->current = n;\n"
		"\tGEN_AddChild(t, old_current);\n"
		"\told_current->parent = (struct GEN_Node *)n;\n"
		"\tGEN_Parent(t);\n"
		"\tif\t(old_current == t->root) {\n"
		"\t\tt->root = n;\n"
		"\t}\n"
		"}\n\n"

		"const char *GEN_CellBordersFormat(GEN_TokenType t)\n"
		"{\n"
		"\tswitch (t) {\n"
		"\t\tdefault: {\n"
		"\t\t\treturn 	\"rectangle\";\n"
		"\t\t}\n"
		"\t}\n"
		"}\n\n"

		"const char *GEN_CheckIfRuleName(GEN_PrsrNdType type)\n"
		"{\n"
		"\tswitch (type) {\n"
		"\t\tcase            GEN_VAR_NAME: { return \"yellow\"; }\n"
		"\t\tcase           GEN_RULE_NAME: { return \"cyan\"; }\n"
		"\t\tcase GEN_RULE_NAME_REFERENCE: { return \"red\"; }\n"
		"\t\tcase  GEN_VAR_NAME_REFERENCE: { return \"green\"; }\n"
		"\t\tdefault: {return \"black\"; }\n"
		"\t}\n"
		"}\n\n"

		"GEN_TokenType GEN_Ttype(GEN_Node *n)\n"
		"{ return n->token->type; }\n\n"

		"void GEN_PrintNode(FILE *f, GEN_Node *n)\n"
		"{\n"
		"\tassert(f != NULL && \"Null param\");\n"
		"\tassert(n != NULL && \"Null param\");\n"
		"\tif (strcmp(n->token->txt, GEN_TranslateTokenType(n->token->type)) == 0) {\n"
		"\t\tfprintf(f, NODE_FMT,\n"
		"\t\tn->id,\n"
		"\t\tGEN_CellBordersFormat(n->token->type),\n"
		"\t\tGEN_CheckIfRuleName(n->token->parser_type),\n"
		"\t\tn->token->txt,\n"
		"\t\t\"\");\n"
		"\t} else {\n"
		"\t\tfprintf(f, NODE_FMT,\n"
		"\t\t\tn->id,\n"
		"\t\t\tGEN_CellBordersFormat(n->token->type),\n"
		"\t\t\tGEN_CheckIfRuleName(n->token->parser_type),\n"
		"\t\t\tn->token->txt,\n"
		"\t\t\tGEN_TranslateTokenType(n->token->type));\n"
		"\t}\n"
		"\tif (n->children != NULL) {\n"
		"\t	for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {\n"
		"\t		GEN_PrintNode(f, GEN_GetChild(n, cur_child));\n"
		"\t	}\n"
		"\t}\n"
		"}\n\n"

		"void GEN_ConnectNode(FILE *f, GEN_Node *n)\n"
		"{\n"
		"\tassert(f != NULL && \"Null param\");\n"
		"\tassert(n != NULL && \"Null param\");\n"
		"\tif (n->children != NULL) {\n"
		"\t\tfor (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {\n"
		"\t\t\tfprintf(f, \"\\tn%%ld -> n%%ld\\n\", n->id, GEN_GetChild(n, cur_child)->id);\n"
		"\t\t}\n"
		"\t}\n"
		"\tif (n->children != NULL) {\n"
		"\t\tfor (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {\n"
		"\t\t\tGEN_ConnectNode(f, GEN_GetChild(n, cur_child));\n"
		"\t\t}\n"
		"\t}\n"
		"}\n\n"

		"void GEN_DebugTree(GEN_Tree *t)\n"
		"{\n"
		"\tassert(t != NULL && \"Null param\");\n"
		"\tFILE *f = fopen(\"../graph.dot\", \"w\");\n"
		"\tassert(f != NULL && \"Reading file error\");\n"
		"\tfprintf(f, \"digraph G{\\n\");\n"
		"\tfprintf(f, \"\\tlabel=\\\"AST of file\\\"\\n\"\n);"
		"\tfprintf(f, \"\tgraph [dpi=50];\\n\\n\");\n"
		"\tGEN_PrintNode(f, t->root);\n"
		"\tfprintf(f, \"\\n\");\n"
		"\tGEN_ConnectNode(f, t->root);\n"
		"\tfprintf(f, \"}\\n\");\n"
		"\tfclose(f);\n"
		"\tsystem(\"dot -Tpng ../graph.dot -o ../graph.png\");\n"
		"}\n\n"

		"void GEN_Parent(GEN_Tree *t)\n"
		"{ t->current = (GEN_Node *)t->current->parent; }\n\n"

		"inline __attribute__((always_inline))\n"
		"GEN_Node *GEN_GetChild(GEN_Node *n, uint64_t idx)\n"
		"{ return (*(GEN_Node **)((char *)n->children->data + idx * sizeof(GEN_Node*))); }\n\n"

		"void GEN_AppendTree(GEN_Tree *first, GEN_Tree *second)\n"
		"{\n"
		"\tGEN_Node  *second_cur = second->current;\n"
		"\tGEN_Node **children   = (GEN_Node **)calloc(second_cur->children->size, sizeof(GEN_Node*));\n"
		"\tfor (size_t cur_child = 0; cur_child < second_cur->children->size; ++cur_child) {\n"
		"\t\tGEN_Node *child = GEN_GetChild(second_cur, cur_child);\n"
		"\t\tmemcpy(children, &child, sizeof(GEN_Node*));\n"
		"\t}\n"
		"\tfor (size_t cur_child = 0; cur_child < second_cur->children->size; ++cur_child) {\n"
		"\t\tGEN_AddChild(first, GEN_GetChild(second_cur, cur_child));\n"
		"\t\tGEN_Parent(first);\n"
		"\t\tGEN_GetChild(first->current, cur_child)->parent = first->current;\n"
		"\t}\n"
		"\tsecond->current =  first->current;\n"
		"}\n\n"

		"GEN_Node *GEN_CreateNodeByType(GEN_Tree *t, GEN_TokenType type)\n"
		"{\n"
		"\tGEN_Node *new_node = GEN_NodeCtor();\n"
		"\tGEN_FillEmptyStr(&new_node->token->txt, GEN_TranslateTokenType(type));\n"
		"\tnew_node->token->type = type;\n"
		"\tnew_node->id = (uint64_t)new_node;\n"
		"\t++t->size;\n"
		"\treturn new_node;\n"
		"}\n\n"

		"GEN_Node *GEN_CreateNode(GEN_Tree *t, GEN_Token *token)\n"
		"{\n"
		"\tGEN_Node *new_node = GEN_NodeCtor();\n"
		"\tGEN_FillEmptyStr(&new_node->token->txt, token->txt);\n"
		"\tnew_node->token->type = token->type;\n"
		"\tnew_node->id = ((uint64_t)new_node);\n"
		"\t++t->size;\n"
		"\treturn new_node;\n"
		"}\n\n"

		"GEN_Array *GEN_ArrayCtor(uint64_t el_sz)\n"
		"{\n"
		"\tGEN_Array *a = (GEN_Array *)calloc(1, sizeof(GEN_Array));\n"
		"\tassert(a != NULL && \"Null calloc allocation\");\n"
		"\ta->size         = 0;\n"
		"\ta->capacity     = 1;\n"
		"\ta->element_size = el_sz;\n"
		"\ta->data = calloc(1, el_sz * sizeof(char));\n"
		"\tassert(a->data != NULL && \"Null calloc allocation\");\n"
		"\treturn a;\n"
		"}\n\n"

		"void GEN_FillEmptyStr(char **dest, const char *src)\n"
		"{\n"
		"\tassert(src  != NULL && \"Null param\");\n"
		"\t(*dest) = (char *)calloc(kTokenMaxLen, sizeof(char));\n"
		"\tassert(dest != NULL && \"Null calloc allocation\");\n"
		"\tstrcpy(*dest, src);\n"
		"}\n\n"

		"void GEN_ArrayChangeCapacity(GEN_Array *a, uint64_t new_capacity)\n"
		"{\n"
		"\tassert(a != NULL && \"Null param\");\n"
		"\ta->data = realloc(a->data, a->element_size * new_capacity * sizeof(char));\n"
		"\tassert(a->data != NULL && \"Null reallocation\");\n"
		"\ta->capacity = new_capacity;\n"
		"}\n\n"

		"void GEN_ArrayAdd(GEN_Array *a, void *new_element)\n"
		"{\n"
		"\tif (a->size == a->capacity) {\n"
		"\t\tGEN_ArrayChangeCapacity(a, a->capacity << 1);\n"
		"\t}\n"
		"\tvoid *ptr = (char *)a->data + a->size * a->element_size;\n"
		"\tmemcpy(ptr, &new_element, a->element_size);\n"
		"\t++a->size;\n"
		"}\n\n"

		"void GEN_CompressTree(GEN_Tree *t, GEN_Node *n)\n"
		"{\n"
		"   t->current = n;\n"
		"	int n_changes = 0;\n"
		"   while(true) {\n"
		"		if (n->children != NULL) {\n"
		"			GEN_Node *save_cur = n;\n"
		"			for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {\n"
		"				GEN_Node *child = GEN_GetChild(n, cur_child);\n"
		"				if (child->children != NULL && child->children->size == 1) {\n"
		"					GEN_Node *grandson = GEN_GetChild(child, 0);\n"
		"					grandson->parent = n;\n"
		"					\n"
		"					++n_changes;\n"
		"					memcpy((GEN_Node**)n->children->data + cur_child, &grandson, sizeof(GEN_Node*));\n"
		"				}\n"
		"			}\n"
		"		}\n"
		"		if (n_changes == 0) {\n"
		"			break;\n"
		"		}\n"
		"		n_changes = 0;\n"
		"	}\n"
		"	if (n->children != NULL) {\n"
		"		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {\n"
		"			GEN_CompressTree(t, GEN_GetChild(n, cur_child));\n"
		"		}\n"
		"	}\n"
		"}\n"
		);

	// fclose(header_file);
	fclose(c_file);
}

void GenerateFiles(Token *s, uint64_t n_tokens)
{
	assert(s != NULL && "Null param");

	Tree *tokenizer_tree = TreeCtor(TOKENIZER_TREE);
	Tree *parser_tree    = TreeCtor(PARSER_TREE);

	GenerateTrees(parser_tree, tokenizer_tree, s, n_tokens);
	// Scan trees for names
	NameTable *table        = ScanTokenizerNames(tokenizer_tree);
	DebugTree(tokenizer_tree);

	NameTable *parser_table = ScanParserNames(parser_tree, table);
	DebugTree(parser_tree);

	FILE *header_file = fopen("../out/Tokenizer_GEN.h", "w");

	/* After tree generation
	generator should generate tokenizer file.
	it will contain */
	GenerateTokenizerFile(tokenizer_tree, table, header_file, parser_table);
	GenerateTreeFile(header_file);

	GenerateParserFile(parser_tree, s, n_tokens, table, header_file);
	DebugTree(parser_tree);
	// exit(0);


	fclose(header_file);
}
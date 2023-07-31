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

void GenerateParserAst(Tree *parser_tree, Tree *tokenizer_tree, Token *s, uint64_t n_tokens)
{
	__asrt(s != NULL, "Null parametr\n");

	__msg(D_PARSER_GENERATING, M,
		"Start of generating parser's AST\n");

	size_t token_idx = 0;
	__msg(D_PARSER_GENERATING, M,
		"Start parsing\n");

	__tab_incr();
	for (; token_idx < n_tokens; ++token_idx) {
		if (s[token_idx].type == TOKEN_PERCENT) {
			__msg(D_PARSER_GENERATING, M,
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
		__tab_decr();

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
			printf("Already exists\n");
			__msg(D_NAMETABLE, M,
				"Already exists\n");

			return cur_el;
		}
	}

	return -1;
}

bool AddName(NameTable *table, Node *n)
{
	__asrt(table != NULL, "Null parametr\n");
	__asrt(n     != NULL, "Null parametr\n");

	// Try to find this n in nametable.
	if (SearchInTable(n, table) != -1) {
		return false;
	}
	// Add new name in table.
	// printf("?\n");
	memcpy(table->names + table->size, &n, sizeof(Node*));
	__msg(D_NAMETABLE, M,
		"Name was inserted\n");

	++table->size;
	__msg(D_NAMETABLE, M,
		"Current size:%ld\n", table->size);

	return true;
}

void SearchNamesInBrance(Node *n, NameTable *table)
{
	__asrt(n     != NULL, "Null parametr\n");
	__asrt(table != NULL, "Null parametr\n");

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
			__tab_incr();
			SearchNamesInBrance(cur_child, table);
			__tab_decr();
			__msg(D_NAMETABLE, M,
				"End this child\n");
		}
	}
}

NameTable *ScanTokenizerNames(Tree *t)
{
	__asrt(t != NULL, "Null parametr\n");
	// Allocate name table for searching familiar names
	NameTable *table = (NameTable*)calloc(1, sizeof(NameTable));
	__asrt(table != NULL, "Null calloc allocation\n");
	// Allocate array of names.
	table->names = (Node**)calloc(kInitSizeNamesArray, sizeof(Node*));
	__asrt(table != NULL, "Null calloc allocation\n");

	__tab_incr();
	SearchNamesInBrance(t->root, table);
	__tab_decr();

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

void GenerateEnumOfTokens(FILE *header_file, Tree *tokenizer_tree, NameTable *table)
{
	fprintf(header_file,
		"typedef enum {\n");

	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el]->token->txt, "splitters") == 0 ||
				strcmp(table->names[cur_el]->token->txt, "start")     == 0 ||
				strcmp(table->names[cur_el]->token->txt, "white_space") == 0) {
			continue;
		}

		fprintf(header_file, "\t%s,\n", table->names[cur_el]->token->txt);
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
		"  // Static value (if exists).\n"
		"  Value     *value;\n"
		"  PrsrNdType parser_type;\n"
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
		// if (label[strlen(label) - 1] == '_') {
		// 	continue;
		// }

		fprintf(header_file, "\t{\"%s\", %ld, %s},\n",
			GetChild(GetChild(table->names[cur_el]->parent, 1), 0)->token->txt,
			strlen(GetChild(GetChild(table->names[cur_el]->parent, 1), 0)->token->txt),
			table->names[cur_el]->token->txt);
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
		"\ttoken.type = TOKEN_NAME;\n"
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
		"\t\t\ttoken = GEN_FillToken(cur_word);\n"
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

void GenerateTranslationCommand(FILE *c_file, Tree *tokenizer_tree, NameTable *table)
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
	fprintf(c_file,
		"\t\tdefault: {\n"
		"\t\t\treturn \"UNKNOWN\";\n"
		"\t\t}\n"
		"\t}\n"
		"\treturn \"UNKNOWN\";\n"
		"}\n\n"
		);
}

void GenerateTokenizerFile(Tree *tokenizer_tree, NameTable *table)
{
	__asrt(tokenizer_tree != NULL, "Null param");
	__asrt(table          != NULL, "Null param");

	FILE *header_file = fopen("../src/Tokenizer_GEN.h", "w");
	FILE *c_file      = fopen("../src/Tokenizer_GEN.c", "w");

	fprintf(header_file,
		"#pragma once\n\n"
		"#include <stdio.h>\n"
		"#include <stdint.h>\n"
		"#include <regex.h>\n"
		"#include <stdlib.h>\n\n"
		"#include <include/RebeccaGenerator.h>\n\n");
	GenerateConstants(header_file, tokenizer_tree, table);
	GenerateEnumOfTokens(header_file, tokenizer_tree, table);
	GenerateArrayOfKeywords(header_file, tokenizer_tree, table);
	fprintf(header_file,
		"GEN_Token *GEN_Tokenizer(char *name, uint64_t *n_tokens);\n");

	fprintf(header_file,
		"const char *GEN_TranslateTokenType(GEN_TokenType type);\n");
	fclose(header_file);
	// C-file:
	fprintf(c_file, "#include <src/Tokenizer_GEN.h>\n\n");
	GenerateSplittersCommands(c_file, tokenizer_tree, table);
	GenerateCommonCommands(c_file, tokenizer_tree, table);

	GenerateTranslationCommand(c_file, tokenizer_tree, table);

	GenerateTokenizerCmd(c_file, tokenizer_tree, table);

	fclose(c_file);
}

void GenerateParserFile(Token *s, uint64_t n_tokens)
{
	__asrt(s != NULL, "Null param");

	Tree *tokenizer_tree = TreeCtor(TOKENIZER_TREE);
	Tree *parser_tree    = TreeCtor(PARSER_TREE);

	GenerateParserAst(parser_tree, tokenizer_tree, s, n_tokens);
	DebugTree(parser_tree);
	// Scan trees for names
	NameTable *table = ScanTokenizerNames(tokenizer_tree);
	DebugTree(tokenizer_tree);
	/* After tree generation
	generator should generate tokenizer file.
	it will contain */

	GenerateTokenizerFile(tokenizer_tree, table);
}
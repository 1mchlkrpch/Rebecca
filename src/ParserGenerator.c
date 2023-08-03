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
static void WorkOutToken(Tree *t, Token *s, uint64_t *idx)
{
	assert(t   != NULL && "Null parametr\n");
	assert(s   != NULL && "Null parametr\n");
	assert(idx != NULL && "Null parametr\n");

	AddChild(t, CreateNode(t, s + *idx));
	++(*idx);
}

/**
 * @brief Checks if the txt data of current node in nametable
 * contain phony variable such as (splitters), (start), (white_space).
 * 
 * @param tokenizer_table    Name tokenizer_table of current node.
 * @param cur_el   Index of curren node in (tokenizer_table).
 * @return         True if that is phony variable.
 */
bool PhonyVariables(NameTable *tokenizer_table, uint64_t cur_el)
{
	return
		strcmp(GetTxt(tokenizer_table->names[cur_el]), "splitters")   == 0 ||
		strcmp(GetTxt(tokenizer_table->names[cur_el]), "start")       == 0 ||
		strcmp(GetTxt(tokenizer_table->names[cur_el]), "white_space") == 0;
}

/**
 * @brief Builds parser's tree and tokenizer's tree.
 * 
 * @param s          Sequence of tokens of YACC-rules-file.
 * @param n_tokens   Number of tokens in sequence.
 */
static void GenerateTrees(Tree *parser_tree, Tree *tokenizer_tree, Token *s, uint64_t n_tokens)
{
	assert(parser_tree    != NULL && "Null parametr\n");
	assert(tokenizer_tree != NULL && "Null parametr\n");
	assert(s              != NULL && "Null parametr\n");

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
			//  Skip this token
			// And next tokens will be: NAME, EQ, char-data.
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
				if (
						s[token_idx].type == TOKEN_SEMICOLON ||
						s[token_idx].type == TOKEN_EOF) {
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

/**
 * @return
 * Index of token with the same text-data as in (n).
 * If strings are equal returns index; if value wasn't found returns (kUndefinedIdx)  
 */
static int64_t SearchInTable(Node *n, NameTable *tokenizer_table)
{
	assert(n     != NULL && "Null parametr\n");
	assert(tokenizer_table != NULL && "Null parametr\n");

	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		//  Search node in nametable with
		// the same data as in (n).
		if (strcmp(GetTxt(tokenizer_table->names[cur_el]), GetTxt(n)) == 0) {
			msg(D_NAMETABLE, M,
				"Already exists\n");

			return cur_el;
		}
	}

	return kUndefinedIdx;
}

/**
 * @brief If node (n) is already in nametable
 * doesn't make any changes. If node (n) not in tokenizer_table
 * add's it to nametable.
 * 
 * @return   true if node (n) wasn't in the tokenizer_table.
 */
static bool AddName(NameTable *tokenizer_table, Node *n)
{
	assert(tokenizer_table != NULL && "Null parametr\n");
	assert(n     != NULL && "Null parametr\n");

	//  Try to find node in the tokenizer_table.
	// If it's already exists return false.
	if (SearchInTable(n, tokenizer_table) != kUndefinedIdx) {
		return false;
	}

	// Append tokenizer_table with new node.
	memcpy(tokenizer_table->names + tokenizer_table->size, &n, sizeof(Node*));
	msg(D_NAMETABLE, M,
		"Name was inserted\n");

	++tokenizer_table->size;
	msg(D_NAMETABLE, M,
		"Current size:%ld\n",
		tokenizer_table->size);

	return true;
}

/**
 * @brief Searchs all names in tokenizer's
 * tree recursively.
 * 
 * @param n       current node to set it's status
 * @param tokenizer_table   Auxilary tokenizer_table of names.
 */
static void SearchTokenizerNamesInBrance(Node *n, NameTable *tokenizer_table)
{
	assert(n     != NULL && "Null parametr\n");
	assert(tokenizer_table != NULL && "Null parametr\n");

	bool is_ref = false;
	if (GetType(n) == TOKEN_NAME) {
		if (
				n->parent         != NULL &&
				n->parent->parent != NULL &&
				GetType(GetChild(n->parent->parent, 0)) == TOKEN_NAME) {
			
			//  Tokenizer's tree is built in a way that all names
			// Contains it's text meaning in
			// 		current->parent->child[1]->child[0].
			// So to check if the name in
			// 		child[0]<-child[1]<-parent<-current
			// was added to nametable we to this check^
			if (SearchInTable(GetChild(n->parent->parent, 0), tokenizer_table) != kUndefinedIdx) {
				// That is not true for nodes with (") or (')-nodes.
				n->token->parser_type = VAR_NAME_REFERENCE;
				is_ref = true;
			}
		}

		// 	If that is new variable name in YACC-file
		// we will add it to name tokenizer_table.
		if (!is_ref) {
			if (AddName(tokenizer_table, n) == true) {
				n->token->parser_type = VAR_NAME;
			} else {
				n->token->parser_type = VAR_NAME_REFERENCE;
			}
		}
	}

	// Call for children recursively.
	if (n->children != NULL) {
		for (size_t cur_child_idx = 0; cur_child_idx < n->children->size; ++cur_child_idx) {
			Node *cur_child = GetChild(n, cur_child_idx);

			tab_incr();
			SearchTokenizerNamesInBrance(cur_child, tokenizer_table);
			tab_decr();

			msg(D_NAMETABLE, M,
				"End this child\n");
		}
	}
}

/**
 * @brief Add's all the tokenizer's names to
 * (tokenizer_table) name tokenizer_table.
 * 
 * @param t   Tokenizer's tree.
 * @return    Name tokenizer_table with all names in (t)-tree.
 */
static NameTable *ScanTokenizerNames(Tree *t)
{
	assert(t != NULL && "Null parametr\n");
	// Allocate name tokenizer_table for searching familiar names
	NameTable *tokenizer_table = (NameTable*)calloc(1, sizeof(NameTable));
	assert(tokenizer_table != NULL && "Null calloc allocation\n");
	// Allocate array of names.
	tokenizer_table->names = (Node**)calloc(kInitSizeNamesArray, sizeof(Node*));
	assert(tokenizer_table != NULL && "Null calloc allocation\n");

	tab_incr();
	SearchTokenizerNamesInBrance(t->root, tokenizer_table);
	tab_decr();

	return tokenizer_table;
}

/**
 * @brief Prints (splitters) and (white_space) constant to
 * (lib_header) file.
 */
static void GenerateConstants(FILE *lib_header, NameTable *tokenizer_table)
{
	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		if (
				strcmp(GetTxt(tokenizer_table->names[cur_el]), "splitters")   == 0 ||
				strcmp(GetTxt(tokenizer_table->names[cur_el]), "white_space") == 0) {
			// Print common c-constant.
			fprintf(lib_header,
				"static const char *GEN_%s = \"%s\";\n",
				GetTxt(tokenizer_table->names[cur_el]),
				GetTxt(GetChild(GetChild(tokenizer_table->names[cur_el]->parent, 1), 0)));
		}
	}

	fprintf(lib_header, "static const int64_t kUndefinedStableWordIdx = -1;\n\n");
}

/**
 * @brief Generates enum of possible types of token
 * in AST of file to parse.
 * 
 * @param lib_header     Libeary header file.
 * @param tokenizer_table          Tokenizer's name tokenizer_table.
 * @param parser_table   Parser's name tokenizer_table.
 */
static void GenerateEnumOfTokens
	(FILE *lib_header, NameTable *tokenizer_table, NameTable *parser_table)
{
	fprintf(lib_header,
		"typedef enum {\n"
		"\tdefault_token,\n"
		);

	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		if (PhonyVariables(tokenizer_table, cur_el)) {
			continue;
		}

		fprintf(lib_header,
			"\t%s,\n",
			GetTxt(tokenizer_table->names[cur_el]));
	}
	for (size_t cur_el = 0; cur_el < parser_table->size; ++cur_el) {
		if (PhonyVariables(parser_table, cur_el)) {
			continue;
		}

		Node *in_parser_tree = parser_table->names[cur_el];
		fprintf(lib_header, "\t%s,\n", GetTxt(in_parser_tree));

		for (size_t cur_child = 0; cur_child < GetChild(in_parser_tree, 0)->children->size; ++cur_child) {
			fprintf(lib_header, "\t%s_%ld,\n", GetTxt(in_parser_tree), cur_child + 1);
		}
	}

	fprintf(lib_header,
		"} GEN_TokenType;\n\n");

	fprintf(lib_header,
		"typedef struct\n"
		"{\n"
		"\t// Type of token.\n"
		"\tGEN_TokenType  type;\n"
		"\t// Text representation of token.\n"
		"\tchar      *txt;\n"
		"\tGEN_PrsrNdType parser_type;\n"
		"} GEN_Token;\n\n");
}

/**
 * @brief Prints array of keywords in library header.
 * Prints special structure (GEN_StableWord) and array of this structure.
 * It will contain all stable words to parser.
 * 
 * @param lib_header   Library header file.
 * @param tokenizer_table        Tokenizer's name tokenizer_table.
 */
static void GenerateArrayOfKeywords(FILE *lib_header, NameTable *tokenizer_table)
{
	fprintf(lib_header,
		"typedef struct {\n"
		"\t// Text representation of token.\n"
		"\tconst char *txt;\n"
		"\t// Length of token.\n"
		"\tsize_t      len;\n"
		"\t// Token logic type.\n"
		"\tGEN_TokenType   type;\n"
		"} GEN_StableWord;\n\n");

	fprintf(lib_header,
		"static GEN_StableWord GEN_stable_words[] = {\n");

	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		if (PhonyVariables(tokenizer_table, cur_el)) {
			continue;
		}

		//  Regex expressions shouldn't be
		// considered as common text-values of token.
		char *label = GetTxt(tokenizer_table->names[cur_el]);
		if (label[strlen(label) - 1] == '_') {
			continue;
		}

		fprintf(lib_header,
			"\t{\"%s\", %ld, %s},\n",
			GetTxt(GetChild(GetChild(tokenizer_table->names[cur_el]->parent, 1), 0)),
			strlen(GetTxt(GetChild(GetChild(tokenizer_table->names[cur_el]->parent, 1), 0))),
			GetTxt(tokenizer_table->names[cur_el]));
	}

	fprintf(lib_header,
		"};\n\n");

	fprintf(lib_header,
		"static GEN_StableWord GEN_stable_regex_words[] = {\n");

	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		if (PhonyVariables(tokenizer_table, cur_el)) {
			continue;
		}

		char *label = tokenizer_table->names[cur_el]->token->txt;
		if (label[strlen(label) - 1] == '_') {
			fprintf(lib_header, "\t{\"%s\", %ld, %s},\n",
				GetTxt(GetChild(GetChild(tokenizer_table->names[cur_el]->parent, 1), 0)),
				strlen(GetTxt(GetChild(GetChild(tokenizer_table->names[cur_el]->parent, 1), 0))),
				GetTxt(tokenizer_table->names[cur_el]));
		}

	}

	fprintf(lib_header,
		"};\n\n");
}

/**
 * @brief Print's common tokenizer's commands.
 * @param tokenizer_c   Tokenizer's c file.
 */
static void GenerateCommonCommands(FILE *tokenizer_c)
{
	fprintf(tokenizer_c,
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
		"\t\tint64_t idx = GEN_IdentifyToken(cur_word);\n"
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

/**
 * @brief Generates splitter commands which can find
 * split symbols or whitespace symbols for generated library.
 * 
 * @param tokenizer_c   Tokenizer's c-file.
 * @param tokenizer_table         Tokenizer's name tokenizer_table.
 */
static void GenerateSplittersCommands(FILE *tokenizer_c, NameTable *tokenizer_table)
{
	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		if (strcmp(tokenizer_table->names[cur_el]->token->txt, "splitters") == 0) {
			fprintf(tokenizer_c,
				"static inline __attribute__((always_inline))\n"
				"bool GEN_IsSplit(const char c)\n"
				"{ return strchr(GEN_splitters, c) != NULL; }\n\n");
			break;
		}
	}

	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		if (strcmp(tokenizer_table->names[cur_el]->token->txt, "white_space") == 0) {
			fprintf(tokenizer_c,
				"static inline __attribute__((always_inline))\n"
				"bool GEN_IsWhiteSpace(const char c)\n"
				"{ return strchr(GEN_white_space, c) != NULL; }\n\n");
			break;
		}
	}
}

/**
 * @brief Prints tokenizer's main command.
 * @param tokenizer_c   Tokenizer's c file.
 */
static void GenerateTokenizerCmd(FILE *tokenizer_c)
{
	fprintf(tokenizer_c,
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

/**
 * @brief Prints trainslation command: token_type -> txt.
 * This command can be used in (GEN_DebugTree)-function to create graph of
 * AST of the program.
 * 
 * @param tokenizer_c       Tokenizer's c file.
 * @param tokenizer_table   Tokenizer's name table
 * @param parser_table      Name table of parser.
 */
static void GenerateTranslationCommand
	(FILE *tokenizer_c, NameTable *tokenizer_table, NameTable *parser_table)
{
	fprintf(tokenizer_c,
		"const char *GEN_TranslateTokenType(GEN_TokenType type)\n"
		"{\n"
		"\tswitch (type) {\n");

	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		if (PhonyVariables(tokenizer_table, cur_el)) {
			continue;
		}

		fprintf(tokenizer_c,
			"\t\tcase %s: { return \"%s\"; }\n",
			GetTxt(tokenizer_table->names[cur_el]),
			GetTxt(tokenizer_table->names[cur_el])
			);
	}

	for (size_t cur_el = 0; cur_el < parser_table->size; ++cur_el) {
		if (PhonyVariables(parser_table, cur_el)) {
			continue;
		}

		Node *in_parser_tree = parser_table->names[cur_el];
		fprintf(tokenizer_c,
			"\t\tcase %s: { return \"%s\"; }\n",
			GetTxt(parser_table->names[cur_el]),
			GetTxt(parser_table->names[cur_el])
			);

		for (size_t cur_child = 0; cur_child < GetChild(in_parser_tree, 0)->children->size; ++cur_child) {
			fprintf(tokenizer_c,
				"\t\tcase %s_%lu: { return \"%s_%lu\"; }\n",
				in_parser_tree->token->txt, cur_child + 1,
				in_parser_tree->token->txt, cur_child + 1
				);
		}
	}

	fprintf(tokenizer_c,
		"\t\tdefault: {\n"
		"\t\t\treturn \"default_token\";\n"
		"\t\t}\n"
		"\t}\n"
		"\treturn \"default_token\";\n"
		"}\n\n"
		);
}

/**
 * @brief Generates Tokenizer's c_file.
 * 
 * @param tokenizer_tree    Tokenizer's tree.
 * @param tokenizer_table   Tokenizer's name table.
 * @param lib_header        Header of library.
 * @param parser_table      Parser's name table.
 */
static void GenerateTokenizerFile
	(Tree *tokenizer_tree, NameTable *tokenizer_table, FILE *lib_header, NameTable *parser_table)
{
	assert(tokenizer_table          != NULL && "Null param");
	assert(tokenizer_tree != NULL && "Null param");
	assert(parser_table   != NULL && "Null param");

	FILE *tokenizer_c = fopen("../out/Tokenizer_GEN.c", "w");

	fprintf(lib_header,
		"#pragma once\n\n"
		"#include <assert.h>\n"
		"#include <stdbool.h>\n"
		"#include <stdio.h>\n"
		"#include <string.h>\n"
		"#include <stdint.h>\n"
		"#include <regex.h>\n"
		"#include <stdlib.h>\n\n"

		"#pragma GCC diagnostic push\n"
		"#pragma GCC diagnostic ignored \"-Wunused-variable\"\n"

		"static const uint64_t kMaxScopeDepth = 256;\n"
		"static const uint64_t kInitSequenceSize = 1024;\n\n"
	
		"typedef enum\n"
		"{\n"
		"\tGEN_NOT_SPECIAL,\n"
		"\t   GEN_VAR_NAME,\n"
		"\t  GEN_RULE_NAME,\n"
		"\t  GEN_RULE_NAME_REFERENCE,\n"
		"\t   GEN_VAR_NAME_REFERENCE,\n"
		"} GEN_PrsrNdType;\n\n"
		);

	GenerateConstants(lib_header, tokenizer_table);
	GenerateEnumOfTokens(lib_header, tokenizer_table, parser_table);

	fprintf(lib_header,
		"typedef struct\n"
		"{\n"
		"\tuint64_t n_parsed;\n"
		"\tuint64_t cur_token_idx;\n"
		"} GEN_Context;\n\n"
		);

	GenerateArrayOfKeywords(lib_header, tokenizer_table);
	fprintf(lib_header,
		"GEN_Token *GEN_Tokenizer(char *name, uint64_t *n_tokens);\n");

	fprintf(lib_header,
		"const char *GEN_TranslateTokenType(GEN_TokenType type);\n");

	fprintf(tokenizer_c,
		"#include <lib_GEN.h>\n\n");

	GenerateSplittersCommands(tokenizer_c, tokenizer_table);
	GenerateCommonCommands(tokenizer_c);

	GenerateTranslationCommand(tokenizer_c, tokenizer_table, parser_table);

	GenerateTokenizerCmd(tokenizer_c);

	fclose(tokenizer_c);
}

// Generation parser file. -------------------------------------------------------------

/**
 * @brief Writes prefix of current functino of particular
 * rule of file.
 * 
 * @param lib_header     Header file of library.
 * @param parser_c       Parser's c file.
 * @param name_of_rule   Name of current rule.
 */
static void WritePrefixOfParserFunction(FILE *lib_header, FILE *parser_c, const char *name_of_rule)
{
	assert(lib_header  != NULL && "Null parametr\n");
	assert(name_of_rule != NULL && "Null parametr\n");

	fprintf(lib_header,
		"GEN_Context Try_%s(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);\n\n",
		name_of_rule
		);

	fprintf(parser_c, "GEN_Context Try_%s(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)\n{\n"
		"\tGEN_Context try_ctx = ctx;\n"
		"\tGEN_Tree %s_tree = {0};\n\n",
		name_of_rule,
		name_of_rule
		);

	fprintf(parser_c,
		"\tGEN_AddChild(&%s_tree, GEN_CreateNodeByType(&%s_tree, %s));\n"
		"\tGEN_Context new_ctx = {0};\n\n",
		name_of_rule,
		name_of_rule,
		name_of_rule
		);
}

static void WriteReferenceToOtherRules(FILE *lib_header, char *tabs, uint64_t cur_child, uint64_t *n_tabs, char *name_of_rule)
{
	tabs[(*n_tabs)++] = '\t';

	fprintf(lib_header,
		"%snew_ctx = Try_%s_%lu(&%s_tree, sequence, try_ctx, n_tokens);\n",
		tabs, name_of_rule, cur_child + 1, name_of_rule);

	fprintf(lib_header,
		"%sif (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {\n",
		tabs);
}

/**
 * @brief Prints all obvious command to parser's file
 * 
 * @param lib_header        Header of library.
 * @param parser_c          Parser's c file
 * @param tokenizer_table   Tokenizer's name table.
 */
static void WriteObviousCommands(FILE *lib_header, FILE *parser_c, NameTable *tokenizer_table)
{
	fprintf(lib_header,
		"GEN_Context GEN_TryToken(GEN_Tree *t, GEN_Token *sequence, GEN_TokenType expected_type, GEN_Context ctx, uint64_t n_tokens);\n");

	fprintf(parser_c,
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

	for (size_t cur_el = 0; cur_el < tokenizer_table->size; ++cur_el) {
		if (strcmp(tokenizer_table->names[cur_el]->token->txt, "start") == 0) {
			fprintf(parser_c,
				"GEN_Tree *ParseSequence(GEN_Tree *t, GEN_Token *s, GEN_Context ctx, int64_t n_tokens) {\n"
				"\tTry_%s(t, s, ctx, n_tokens);\n"
				"\treturn t;\n"
				"}\n\n",
				GetTxt(GetChild(GetChild(tokenizer_table->names[cur_el]->parent, 1), 0))
				);

			fprintf(lib_header,
				"GEN_Tree *ParseSequence(GEN_Tree *t, GEN_Token *s, GEN_Context ctx, int64_t n_tokens);\n");
			break;
		}

	}
}

/**
 * @brief Prints one chain in sequence of tokence in line
 * of YACC-rule line.
 * 
 * @param parser_c       Parser's c file.
 * @param chain          Current node in parser's AST.
 * @param tabs           Number of tabs to print before each line.
 * @param cur_child      Index of current child (=line index of current rule).
 * @param name_of_rule   Name of current rule in parser's AST.
 */
static void WriteChain
	(FILE *parser_c, Node *chain, char *tabs, uint64_t cur_child, char *name_of_rule)
{
	msg(D_FILE_PRINT, M,
		"Current node in line:%s\n", chain->token->txt);

	fprintf(parser_c,
		"%stab_incr();\n"
		"%smsg(D_PARSER_WORK, M, \"new chain in option\\n\");\n"
		"%stab_incr();\n",
		tabs, tabs, tabs);

	if (chain->token->parser_type == NOT_SPECIAL) {
		fprintf(parser_c,
			"%snew_ctx = GEN_TryToken(&%s_%lu_tree, sequence, %s, try_ctx, n_tokens);\n",
			tabs, name_of_rule, cur_child + 1, TranslateTokenType(GetType(chain)));

	} else if (chain->token->parser_type == RULE_NAME_REFERENCE) {
		fprintf(parser_c,
			"%snew_ctx = Try_%s(&%s_%lu_tree, sequence, try_ctx, n_tokens);\n",
			tabs, chain->token->txt, name_of_rule, cur_child + 1);
	} else if (chain->token->parser_type == VAR_NAME_REFERENCE) {
		fprintf(parser_c,
			"%snew_ctx = GEN_TryToken(&%s_%lu_tree, sequence, %s, try_ctx, n_tokens);\n",
			tabs, name_of_rule, cur_child + 1, chain->token->txt);
	}

	fprintf(parser_c,
		"%stab_decr();\n",
		tabs);

	fprintf(parser_c,
		"%sif (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {\n"
		"%s\tmsg(D_PARSER_WORK, M, \"NOT NEEDED OPTION: Try_%s_%lu\\n\");\n"
		"%s\ttab_decr();\n"
		"%s\treturn ctx;\n"
		"%s}\n"
		"%stry_ctx = new_ctx;\n",
		tabs, tabs, name_of_rule, cur_child + 1, tabs, tabs, tabs, tabs);

	fprintf(parser_c,
		"%smsg(D_PARSER_WORK, M, \"END chain in option\\n\");\n"
		"%stab_decr();\n\n",
		tabs, tabs);
}

/**
 * @brief Prints one parser's command using parser's AST tree.
 * 
 * @param lib_header   Header of library.
 * @param parser_c     Parser's c file.
 * @param n            Current node in parser's tree
 */
static void GenerateCommand(FILE *lib_header, FILE *parser_c, Node *n)
{
	assert(lib_header != NULL && "Null parametr\n");
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
		fprintf(lib_header,
			"GEN_Context Try_%s_%ld(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens);\n\n",
			name_of_rule, cur_child + 1);
		// Firstly we will write all the options of this rule to the parser file.
		fprintf(parser_c,
			"GEN_Context Try_%s_%ld(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)\n{\n",
			name_of_rule, cur_child + 1);

		tabs[n_tabs++] = '\t';

		fprintf(parser_c,
			"%smsg(D_PARSER_WORK, M, \"%s\\n\");\n",
			tabs, name_of_rule);

		fprintf(parser_c,
			"%sGEN_Context try_ctx = ctx;\n"
			"%sGEN_Tree %s_%ld_tree = {0};\n",
			tabs, tabs, name_of_rule, cur_child + 1);

		fprintf(parser_c,
			"%sGEN_AddChild(&%s_%lu_tree, GEN_CreateNodeByType(&%s_%lu_tree, %s_%lu));\n"
			"%sGEN_Context new_ctx = {0};\n\n",
			tabs, name_of_rule, cur_child + 1,
			name_of_rule, cur_child + 1,
			name_of_rule, cur_child + 1,
			tabs);

		Node *chain = GetChild(fork, cur_child);
		tab_incr();
		while (chain->children != NULL) {
			WriteChain(parser_c, chain, tabs, cur_child, name_of_rule);
			chain = GetChild(chain, 0);
		}
		// 	Write last chain in sequence in particular
		// line in rule.
		WriteChain(parser_c, chain, tabs, cur_child, name_of_rule);
		fprintf(parser_c,
			"\tGEN_AppendTree(t, &%s_%lu_tree);\n",
			name_of_rule, cur_child + 1);

		tab_decr();
		msg(D_FILE_PRINT, M,
					"end of line\n");

		fprintf(parser_c,
			"%smsg(D_PARSER_WORK, M, \"EXACTLY Try_%s_%lu\\n\");\n",
			tabs, name_of_rule, cur_child + 1);

		fprintf(parser_c,
			"%sGEN_InsertParent(t, GEN_CreateNode(t, sequence + n_tokens - 1));\n",
			tabs);

		--n_tabs;
		tabs[n_tabs] = '\0';

		fprintf(parser_c, "\treturn try_ctx;\n");
		fprintf(parser_c, "}\n\n");
	}

	/* Secondly print prefix of parser
	function that contains local GEN_context - if that function
	can be applied to the current place in sequence of token
	this GEN_context will commit it and contain local progress
	in parsing sequence.*/
	WritePrefixOfParserFunction(lib_header, parser_c, name_of_rule);

	msg(D_PARSER_GENERATING, M,
		"Not tabs:\"%s\"(%lu)\n",
		tabs, n_tabs);

	msg(D_PARSER_GENERATING, M,
		"Current rule has (%lu) options\n",
		fork->children->size);

	for (uint64_t cur_child = 0; cur_child < fork->children->size; ++cur_child) {
		WriteReferenceToOtherRules(parser_c, tabs, cur_child, &n_tabs, name_of_rule);
	}

	/* If all rules can't be applied to the sequence that was wrong rule
	to parse current place in token sequence so we return original contest.*/
	fprintf(parser_c, "%s\ttab_decr();\n", tabs);
	fprintf(parser_c, "%s\treturn ctx;\n", tabs);

	// Write all close-braces for all ifs.
	for (uint64_t cur_child = 0; cur_child < fork->children->size; ++cur_child) {
		fprintf(parser_c,
			"%s}\n",
			tabs);

		--n_tabs;
		tabs[n_tabs] = '\0';
	}

	/* If one rule-option can be applied to the token sequence
	we can add it's tree that was build by this option to the parent tree.*/
	fprintf(parser_c,
		"\n\tGEN_AppendTree(t, &%s_tree);\n"
		"\treturn new_ctx;\n", name_of_rule);

	fprintf(parser_c, "}\n\n");
}

/**
 * @brief Prints all the parser's commands recursively.
 * 
 * @param lib_header   Header of library.
 * @param parser_c     Parser's c file.
 * @param n            Current node in parser's tree.
 */
static void GenerateCommands(FILE *lib_header, FILE *parser_c, Node *n)
{
	assert(lib_header != NULL && "Null parametr\n");
	assert(parser_c   != NULL && "Null parametr\n");
	assert(n          != NULL && "Null parametr\n");

	if (n->token->parser_type == RULE_NAME) {
		// Check if it's rule root node.
		msg(D_FILE_PRINT, M,
			"Found rule-node:\"%s\"\n",
			n->token->txt);
		//  If that's rule name we will
		// generate function text and print it
		// to the parser file.
		tab_incr();
		GenerateCommand(lib_header, parser_c, n);
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
				GenerateCommands(lib_header, parser_c, GetChild(n, cur_child));
				tab_decr();
				msg(D_FILE_PRINT, M,
					"End of parsing children\n");
			}
		}
	}
}

/**
 * @brief Searches all other (not rule-names) in parser's AST
 * And add's it's parser's type to that node.
 * Work's recursively.
 * 
 * @param n                 Current node in parser's AST.
 * @param tokenizer_table   Tokenizer's name table.
 * @param parser_table      Parser's name table
 */
static void SearchNamesInBranceParser
	(Node *n, NameTable *tokenizer_table, NameTable *parser_table)
{
	assert(n     != NULL && "Null parametr\n");
	assert(tokenizer_table != NULL && "Null parametr\n");

	bool is_ref = false;
	if (GetType(n) == TOKEN_NAME) {
		if (SearchInTable(n, tokenizer_table) != -1) {
			n->token->parser_type = VAR_NAME_REFERENCE;
			is_ref = true;
		}

		if (!is_ref) {
			if (GetType(n) == TOKEN_NAME &&
					n->children != NULL &&
					GetType(GetChild(n,0)) == TOKEN_COLON) {
				n->token->parser_type = RULE_NAME;
			} else {
				n->token->parser_type = RULE_NAME_REFERENCE;
			}
		}
	}

	if (n->children != NULL) {
		for (size_t cur_child_idx = 0; cur_child_idx < n->children->size; ++cur_child_idx) {
			Node *cur_child = GetChild(n, cur_child_idx);
			tab_incr();
			SearchNamesInBranceParser(cur_child, tokenizer_table, parser_table);
			tab_decr();
			msg(D_NAMETABLE, M,
				"End this child\n");
		}
	}
}

/**
 * @brief Finds all names in parser's tree by
 * searching for situation: current node is node with
 * type (TOKEN_NAME) and it has only one child: (TOKEN_COLON).
 */
static void AddRuleNames(Node *n, NameTable *parser_table)
{
	if (GetType(n) == TOKEN_NAME &&
			n->children != NULL &&
			GetType(GetChild(n,0)) == TOKEN_COLON) {
		// Add new rule name in (parser_table).
		AddName(parser_table, n);
	}

	// Call this fuction recursively for all children.
	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			AddRuleNames(GetChild(n, cur_child), parser_table);
		}
	}
}

/**
 * @brief Search all parser's
 * names and set's all parser's types for two pass. First pass is for searching all
 * the rule names. It is needed to make recursive descent.
 * Second pass sets all other parser's types.
 * 
 * @param parser_tree       Parser's tree to scan for names
 * @param tokenizer_table   Tokenizer's tokenizer_table to extract name
 * and meanings of it's variables.
 * @return                  Created parser's tokenizer_table
 */
NameTable *ScanParserNames(Tree *parser_tree, NameTable *tokenizer_table)
{
	assert(parser_tree != NULL && "Null parametr\n");
	assert(tokenizer_table       != NULL && "Null parametr\n");
	// Allocate name tokenizer_table for searching familiar names
	NameTable *parser_table = (NameTable*)calloc(1, sizeof(NameTable));
	assert(parser_table != NULL && "Null calloc allocation\n");
	// Allocate array of names.
	parser_table->names = (Node**)calloc(kInitSizeNamesArray, sizeof(Node*));
	assert(parser_table != NULL && "Null calloc allocation\n");

	AddRuleNames(parser_tree->root, parser_table);

	tab_incr();
	SearchNamesInBranceParser(parser_tree->root, tokenizer_table, parser_table);
	tab_decr();

	return parser_table;
}

/**
 * @brief Prints the whole parser's file.
 * 
 * @param parser_tree       AST tree of parser
 * @param tokenizer_table   Tokenizer's table.
 * @param lib_header        Header of library.
 */
static void GenerateParserFile
	(Tree *parser_tree, NameTable *tokenizer_table, FILE *lib_header)
{
	FILE *parser_c = fopen("../out/Parser_GEN.c", "w");

	fprintf(parser_c,
		"#include <../MchlkrpchLogger/logger.h>\n\n"
		"#include <lib_GEN.h>\n\n"
		);

	// Print common commands.
	WriteObviousCommands(lib_header, parser_c, tokenizer_table);

	// Add all parser's command to (parser_c)-file.
	tab_incr();
	GenerateCommands(lib_header, parser_c, parser_tree->root);
	tab_decr();

	fclose(parser_c);
}

/**
 * @brief Prints the whole Tree_GEN.c file and add's it's
 * commands to (lib_header)-file.
 * 
 * @param lib_header   Header file of library.
 */
static void GenerateTreeFile(FILE *lib_header)
{
	FILE *c_file = fopen("../out/Tree_GEN.c", "w");

	fprintf(lib_header,
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

	fprintf(lib_header,
		"void GEN_CompressTree(GEN_Tree *t, GEN_Node *n);\n\n");

	fprintf(c_file,
		"#include <lib_GEN.h>\n"
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
		);

	fprintf(c_file,
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
		);

	fprintf(c_file,
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
		"\tt->current = n;\n"
		"\tint n_changes = 0;\n"
		"\twhile(true) {\n"
		"\tif (n->children != NULL) {\n"
		"\t\tGEN_Node *save_cur = n;\n"
		"\t\tfor (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {\n"
		"\t\t\tGEN_Node *child = GEN_GetChild(n, cur_child);\n"
		"\t\t\tif (child->children != NULL && child->children->size == 1) {\n"
		"\t\t\t\tGEN_Node *grandson = GEN_GetChild(child, 0);\n"
		"\t\t\t\tgrandson->parent = n;\n\n"
		"\t\t\t\t++n_changes;\n"
		"\t\t\t\tmemcpy((GEN_Node**)n->children->data + cur_child, &grandson, sizeof(GEN_Node*));\n"
		"\t\t\t}\n"
		"\t\t}\n"
		"\t}\n"
		"\tif (n_changes == 0) {\n"
		"\t\tbreak;\n"
		"\t}\n"
		"\tn_changes = 0;\n"
		"\t}\n"
		"\tif (n->children != NULL) {\n"
		"\t\tfor (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {\n"
		"\t\t\tGEN_CompressTree(t, GEN_GetChild(n, cur_child));\n"
		"\t\t}\n"
		"\t}\n"
		"}\n"
		);

	fclose(c_file);
}

/**
 * @brief Main function to generate
 * parser's files to parser the program.
 * 
 * @param s          Sequence of tokens.
 * @param n_tokens   Number of tokens in token's sequence
 * collected from tokenizer of YACC-file.
 */
void GenerateFiles(Token *s, uint64_t n_tokens)
{
	assert(s != NULL && "Null param");

	// Create trees with fictive root-node.
	Tree *tokenizer_tree = TreeCtor(TOKENIZER_TREE);
	Tree *parser_tree    = TreeCtor(PARSER_TREE);

	// Generate tokenizer's and parser's tree.
	GenerateTrees(parser_tree, tokenizer_tree, s, n_tokens);

	NameTable *tokenizer_table = ScanTokenizerNames(tokenizer_tree);
	NameTable *parser_table    = ScanParserNames(parser_tree, tokenizer_table);

	FILE *lib_header = fopen("../out/lib_GEN.h", "w");

	//  After tree generation
	// generator should generate tokenizer file.
	// it will contain 
	GenerateTokenizerFile(tokenizer_tree, tokenizer_table, lib_header, parser_table);
	GenerateTreeFile(lib_header);

	GenerateParserFile(parser_tree, tokenizer_table, lib_header);
	DebugTree(parser_tree);

	fclose(lib_header);
}
#include <stdint.h>
#include <stdio.h>

#include <include/RebeccaCompiler.h>
#include <MchlkrpchLogger/logger.h>

// Reading part ----------------------------------------------------------------------------

void ReadTheRestTokensInLine(Tree *t, Token *sequence, uint64_t *cur_token_idx)
{
	__asrt(t             != NULL, "Null param");
	__asrt(sequence      != NULL, "Null param");
	__asrt(cur_token_idx != NULL, "Null param");

	while (sequence[*cur_token_idx].type != TOKEN_SEMICOLON) {
		/* If it's just a PIPE token it's dummy-token of
		YACC-files syntax. So we just skip it.*/
		__msg(D_PARSER_GENERATING, M,
			"Current token in line:\"%s\"\n",
			sequence[*cur_token_idx].txt);

		// Pipe token parsing.
		if (sequence[*cur_token_idx].type == TOKEN_PIPE) {
			__msg(D_PARSER_GENERATING, M,
				"Skip the token.\n");
			// Skip pipe token.
			++(*cur_token_idx);
			continue;
		}

		Node *old_current = t->current;
		__msg(D_PARSER_GENERATING, M,
			"Save the \'t->current\':\"%s\"\n",
			old_current->token->txt);

		// Rule's particular line parsing.
		__tab_incr();
		while (true) {
			if (sequence[*cur_token_idx].type == TOKEN_PIPE ||
			    sequence[*cur_token_idx].type == TOKEN_SEMICOLON) {
				// Stop to parse this line.
				__msg(D_PARSER_GENERATING, M,
					"Next token is \'PIPE\' or \'SEMICOLON\'\n");
				break;
			}

			__msg(D_PARSER_GENERATING, M,
				"Wait until the quote appeared\n");
			/* If quote was met we push last token and return
			old current node as t->current.*/
			if (sequence[*cur_token_idx].type == TOKEN_SINGLE_QUOTE ||
				sequence[*cur_token_idx].type == TOKEN_DOUBLE_QUOTE) {

				ObjType tp = (sequence[*cur_token_idx].type == TOKEN_DOUBLE_QUOTE)?
					OBJ_STRING :
					OBJ_SYM;
				/* Parse quoted argument: we
				should split quotes and add it's data to new token.*/
				__msg(D_PARSER_GENERATING, M,
					"Add quoted data\n");
				// Skip quote.
				++(*cur_token_idx);
				Node *new_rule_option = CreateNode(t, sequence + *cur_token_idx);
				__msg(D_PARSER_GENERATING, M,
					"data:\"%s\"\n",
					new_rule_option->token->txt);

				new_rule_option->token->value = (Value*)calloc(1, sizeof(Value));
				__asrt(new_rule_option->token->value != NULL, "Null calloc allocation");
				new_rule_option->token->value->obj = calloc(strlen(new_rule_option->token->txt), sizeof(char));
				new_rule_option->token->value->type = tp;
				strcpy(new_rule_option->token->value->obj, new_rule_option->token->txt);

				AddChild(t, new_rule_option);
				++(*cur_token_idx);
				++(*cur_token_idx);
				__msg(D_PARSER_GENERATING, M,
					"Now current token is:\"%s\"\n",
					(sequence + *cur_token_idx)->txt);
				continue;
			}

			Node *new_rule_option = CreateNode(t, sequence + *cur_token_idx);
			AddChild(t, new_rule_option);
			++(*cur_token_idx);
		}
		__tab_decr();

		t->current = old_current;
	}
}

/**
 * @brief Grammar rules has consequent structure:
 * 
 * NameOfRule     --- unique name of particular rule
 *   : line-1     <--*
 *   | line-2     <---*----- sequence of tokens
 *   ...             /
 *   | line-n     <-*
 *   ;
 * 
 * Line is sequence of names or data in single or double quotes.
 * In 'GenerateParserFile' we parse 'NameOfRule'-token,
 * by the 'COLON'-token that it's grammar rule structure in YACC-file,
 * we start parsing lines until we meet 'SEMICOLON'-token.
 * Each line we read until we meet 'PIPE'-token or 'SEMICOLON'-token.
 * 
 * After that we insert brunces in 't'-tree.
 * 
 * @param t               Tree to append.
 * @param sequence        Sequence of tokens.
 * @param cur_token_idx   Index of current token in sequence.
 */
void ReadGrammarRule(Tree *t, Token *sequence, uint64_t *cur_token_idx)
{
	__asrt(t             != NULL, "Null param");
	__asrt(sequence      != NULL, "Null param");
	__asrt(cur_token_idx != NULL, "Null param");

	__tab_incr();
	__msg(D_PARSER_GENERATING, M,
		"Read new node:\"%s\"\n",
		(sequence + *cur_token_idx)->txt);

	/* Read first consequent child
	in rule-hierarchy.*/
	Node *primal_node = t->current;
	Node *or_node = CreateNode(t, sequence + *cur_token_idx);
	AddChild(t, or_node);
	++(*cur_token_idx);

	/* Lines of grammar rule is
	Cosequent grammar entities after particular
	grammar entity, for example:
		file
	  	: struct    -*
	  	| funcion   -|
			| variable  -*----lines
	  	;
	So we will parse tokens untill we meet ';'-token.*/
	__msg(D_PARSER_GENERATING, M,
		"Continue to parse lines of grammar rule\n");
	__tab_incr();
	// Start to parse the rest of ther rule.
	ReadTheRestTokensInLine(t,sequence, cur_token_idx);
	// Stop parse lines.
	__tab_decr();

	t->current = primal_node;
	// Returns old current node in tree.
	__msg(D_PARSER_GENERATING, M,
		"Now current in tree is:\"%s\"\n",
		t->current->token->txt);
	__tab_decr();
}

/**
 * @brief Reads the rest of definition after 'NAME'-token.
 * Firstly read 'TOKEN_EQ'-symbol then append tree with cosequent data
 * (from sigle or double comma).
 * 
 * @param t               Tree to append with new definition-brunce.
 * @param sequence        Token sequence to take cosequent tokens from it.
 * @param cur_token_idx   Index of current token in sequence.
 */
void ReadDefinition(Tree *t, Token *sequence, uint64_t *cur_token_idx)
{
	__asrt(t             != NULL, "Null parametr\n");
	__asrt(sequence      != NULL, "Null parametr\n");
	__asrt(cur_token_idx != NULL, "Null parametr\n");

	// Insert 'TOKEN_EQ' token in tree.
	Node *equal_node = CreateNode(t, sequence + *cur_token_idx);
	InsertParent(t, equal_node);
	++(*cur_token_idx);

	ReadTheRestTokensInLine(t, sequence, cur_token_idx);
}

/**
 * @brief Builds AST of YACC-rules from grammar-file.
 * To generate parser file we should general firstly (from simple parser)
 * abstract syntax tree which contains all grammar rules and constant definitions.
 * 
 * This AST is much easy to convert to prepared parser file of
 * programming language.
 * 
 * @param sequence   Token sequence of grammar file.
 * @param n_tokens   Number of tokens in 'sequence'.
 * @returns          Generated AST.
 */
Tree *GenerateParserAst(Token *sequence, uint64_t n_tokens)
{
	__asrt(sequence != NULL, "Null parametr\n");

	__msg(D_PARSER_GENERATING, M, "Start of generating parser's AST\n");
		// Create new ast tree.
	Tree *t = (Tree *)calloc(1, sizeof(Tree));
	assert(t != NULL && "Null calloc allocation");

	bool previous_token_is_name = false;

		/* Add the root of tree as 'EOF'-token.
		It's easier to add new nodes to dummy node.*/ 
	__msg(D_PARSER_GENERATING, M, "Tree's root init\n");	
	Node *common_node = CreateNode(t, sequence + n_tokens - 1);
	AddChild(t, common_node);

	__msg(D_PARSER_GENERATING, M, "Start parsing\n");
	__tab_incr();
	size_t cur_token_idx = 0;
	for (; cur_token_idx < n_tokens; ++cur_token_idx) {
		__msg(D_PARSER_GENERATING, M,
			"Current token:\"%s\"\n", sequence[cur_token_idx].txt);

			/* Check if the current token is name.
			If it is it can be followed by rule-expression
			or definition.*/
		if (sequence[cur_token_idx].type == TOKEN_NAME) {
			__tab_incr();
			__msg(D_PARSER_GENERATING, M,
				"Current token is Name\n");

			// Add child to tree.
			Node *new_node = CreateNode(t, sequence + cur_token_idx);
			AddChild(t, new_node);
			// Check after inserting new node in tree.
			__msg(D_PARSER_GENERATING, M,
				"Token with data:\"%s\" was added\n",
				new_node->token->txt);

				/* Set this flag as true to realize that
				construction NAME + '=' + ... can be grammar rule or definition.*/
			previous_token_is_name = true;
				// Parse new token.
			__msg(D_PARSER_GENERATING, M,
				"Last token is \'previous_token_is_name\' = true(%d)\n",
				previous_token_is_name);
			__tab_decr();
			continue;
		}

		if (previous_token_is_name) {
			__tab_incr();
			previous_token_is_name = false;
				/* Name token may be inheritated by
				text definition (by '='-symbol) or grammar rule (':'-symbol*/
			switch (sequence[cur_token_idx].type) {
					// Grammar rule.
				case TOKEN_COLON: {
					t->current->rule_name = true;
					t->current->token->parser_type = RULE_NAME;
					__msg(D_PARSER_GENERATING, C,
						"Current:\"%s\" is rule_name\n",
						t->current->token->txt);

					__msg(D_PARSER_GENERATING, M,
						"Previous name followed by rule\n");

					ReadGrammarRule(t, sequence, &cur_token_idx);
						/* Move to parent to
						add new rules and definitions to the tree.*/
					Parent(t);
					__msg(D_PARSER_GENERATING, M,
						"Current parent after \'ReadGrammarRule\' is:\"%s\"\n",
						t->current->token->txt);
					break;
				}
					// Text definition.
				case TOKEN_EQ: {
					t->current->token->parser_type = VAR_NAME;
					ReadDefinition(t, sequence, &cur_token_idx);
					Parent(t);
					break;
				}

				default:{
					__tab_decr();
					continue;
				}
			}
				// Stop parse grammar structure.
			__tab_decr();
		}
	}
		// Stop parse grammar rules.
	__tab_decr();
	return t;
}

int64_t ExistInTable(NameTable *table, Token *token)
{
	for (int64_t cur_el = 0; cur_el < (int64_t)table->size; ++cur_el) {
		if (strcmp(table->names[cur_el].txt, token->txt) == 0) {
			__msg(D_NAMETABLE, M,
				"Already exists\n");
			return cur_el;
		}
	}

	return -1;
}

void AddName(NameTable *table, Token *token)
{
	__asrt(table != NULL, "Null parametr\n");
	__asrt(token != NULL, "Null parametr\n");

	// Try to find this token in nametable.
	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		if (strcmp(table->names[cur_el].txt, token->txt) == 0) {
			__msg(D_NAMETABLE, M,
				"Already exists\n");
			return;
		}
	}
	// Add new name in table.
	memcpy(&table->names[table->size], token, sizeof(Token));
	__msg(D_NAMETABLE, M,
		"Name was inserted\n");

	++table->size;
	__msg(D_NAMETABLE, M,
		"Current size:%ld\n", table->size);
}

void SearchNamesInBrance(Node *n, NameTable *table, Node **start_node)
{
	__asrt(n     != NULL, "Null parametr\n");
	__asrt(table != NULL, "Null parametr\n");

	__msg(D_NAMETABLE, M,
		"Check node(%s)\n", n->token->txt);

	if (n->children == NULL) {
		int64_t idx = ExistInTable(table, n->token);
		__msg(D_NAMETABLE, C,
			"Found in table(%s) maybe reference\n", n->token->txt);
		if (idx != -1) {
			if (table->names[idx].parser_type == RULE_NAME) {
				__msg(D_NAMETABLE, M,
				"It's RULE REF\n");
				n->token->parser_type = RULE_NAME_REFERENCE;	
			}

			if (table->names[idx].parser_type == VAR_NAME) {
				__msg(D_NAMETABLE, M,
				"It's NAME REF\n");
				n->token->parser_type = VAR_NAME_REFERENCE;	
			}
		} else {
			if (n->token->value == NULL && n->token->type == TOKEN_NAME) {
				n->token->parser_type = VAR_NAME;
				__msg(D_NAMETABLE, M,
					"it's variable\n");
			}
		}

	} else {
		if (GetChild(n, 0)->token->type == TOKEN_COLON) {
			n->token->parser_type = RULE_NAME;
		}

		int64_t idx = ExistInTable(table, n->token);
		__msg(D_NAMETABLE, C,
			"Found in table(%s) maybe reference\n", n->token->txt);
		if (idx != -1) {
			if (table->names[idx].parser_type == RULE_NAME) {
				__msg(D_NAMETABLE, M,
				"It's RULE REF\n");
				n->token->parser_type = RULE_NAME_REFERENCE;	
			}

			if (table->names[idx].parser_type == VAR_NAME) {
				__msg(D_NAMETABLE, M,
				"It's NAME REF\n");
				n->token->parser_type = VAR_NAME_REFERENCE;	
			}
		}
	}

	if (n->token->value == NULL && n->token->type == TOKEN_NAME) {
		__msg(D_PARSER_GENERATING, M,
			"It is name\n");

		if (strcmp(n->token->txt, "start") == 0) {
			__msg(D_NAMETABLE, C,
					"Start node was found!\n");
			Node *copy = GetChild(n->parent, 1);
			memcpy(start_node, &copy, sizeof(Node*));
			// start_node = GetChild(n->parent, 1);
			__msg(D_NAMETABLE, C,
					"Start has data:\"%s\"\n", (*start_node)->token->txt);
		}

		AddName(table, n->token);
	} else {
		__msg(D_PARSER_GENERATING, M,
			"Not name\n");
	}

	if (n->children != NULL) {
		for (size_t cur_child_idx = 0; cur_child_idx < n->children->size; ++cur_child_idx) {
			Node *cur_child = GetChild(n, cur_child_idx);
			__tab_incr();
			SearchNamesInBrance(cur_child, table, start_node);
			__tab_decr();
			__msg(D_NAMETABLE, M,
				"End this child\n");
		}
	}

	__msg(D_NAMETABLE, M,
		"End(%s)-node\n", n->token->txt);
}

NameTable *ScanForNames(Tree *t, char *txt)
{
	__asrt(t != NULL, "Null parametr\n");

	// Allocate name table for searching familiar names
	NameTable *table = (NameTable*)calloc(1, sizeof(NameTable));
	__asrt(table != NULL, "Null calloc allocation\n");
	// Allocate array of names.
	table->names = (Token*)calloc(kInitSizeNamesArray, sizeof(Token));
	__asrt(table != NULL, "Null calloc allocation\n");

	Node *start_node = NodeCtor();

	__tab_incr();
	SearchNamesInBrance(t->root, table, &start_node);
	__msg(D_NAMETABLE, C,
		"Serching names is over and start entity is:\"%s\"\n",
		start_node->token->txt);
	__tab_decr();

	strcpy(txt, start_node->token->txt);

	return table;
}

// Generating part -------------------------------------------------------------------------

TokenType PeekNextToken(Token *sequence, uint64_t idx)
{ return sequence[idx].type; }

void WritePrefixOfParserFunction(FILE *header_file, FILE *c_file, const char *name_of_rule)
{
	__asrt(header_file            != NULL, "Null parametr\n");
	__asrt(name_of_rule != NULL, "Null parametr\n");

	fprintf(header_file,
		"Context Try_%s(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);\n"
		, name_of_rule);

	fprintf(c_file, "Context Try_%s(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)\n{\n"
		"\tContext try_ctx = ctx;\n"
		"\tTree %s_tree = {0};\n\n", name_of_rule, name_of_rule);

	fprintf(c_file,
		"\tAddChild(&%s_tree, CreateNode(&%s_tree, sequence + n_tokens - 1));\n"
		"\tContext new_ctx = {0};\n\n", name_of_rule, name_of_rule);
}

void WriteReferenceToOtherRules(FILE *header_file, char *tabs, uint64_t cur_child, uint64_t *n_tabs, char *name_of_rule)
{
	tabs[(*n_tabs)++] = '\t';

	fprintf(header_file,
		"%snew_ctx = Try_%s_%lu(&%s_tree, sequence, try_ctx, n_tokens);\n",
		tabs, name_of_rule, cur_child + 1, name_of_rule);

	fprintf(header_file,
		"%sif (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {\n",
		tabs);
}

void WriteChain(FILE *c_file, Node *chain, char *tabs, uint64_t cur_child, char *name_of_rule)
{
	__msg(D_FILE_PRINT, M,
		"Current node in line:%s\n", chain->token->txt);

	fprintf(c_file,
		"%s__tab_incr();\n"
		"%s__msg(D_PARSER_WORK, M, \"new chain in option\\n\");\n"
		"%s__tab_incr();\n",
		tabs, tabs, tabs);

	if (chain->token->parser_type == NOT_SPECIAL) {
		fprintf(c_file,
			"%snew_ctx = TryToken(&%s_%lu_tree, sequence, %s, try_ctx, n_tokens);\n",
			tabs, name_of_rule, cur_child + 1, TranslateTokenType(chain->token->type));

	} else if (chain->token->parser_type == RULE_NAME_REFERENCE) {
		fprintf(c_file,
			"%snew_ctx = Try_%s(&%s_%lu_tree, sequence, try_ctx, n_tokens);\n",
			tabs, chain->token->txt, name_of_rule, cur_child + 1);
	} else if (chain->token->parser_type == VAR_NAME_REFERENCE) {
		fprintf(c_file,
			"%snew_ctx = TryToken(&%s_%lu_tree, sequence, TOKEN_NAME, try_ctx, n_tokens);\n",
			tabs, name_of_rule, cur_child + 1);
	}

	fprintf(c_file,
		"%s__tab_decr();\n",
		tabs);

	fprintf(c_file,
		"%sif (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {\n"
		"%s\t__msg(D_PARSER_WORK, M, \"NOT NEEDED OPTION: Try_%s_%lu\\n\");\n"
		"%s\t__tab_decr();\n"
		"%s\treturn ctx;\n"
		"%s}\n"
		"%stry_ctx = new_ctx;\n",
		tabs, tabs, name_of_rule, cur_child + 1, tabs, tabs, tabs, tabs);

	fprintf(c_file,
		"%s__msg(D_PARSER_WORK, M, \"END chain in option\\n\");\n"
		"%s__tab_decr();\n\n",
		tabs, tabs);
}

void GenerateCommand(FILE *header_file, FILE *c_file, Node *n)
{
	__asrt(header_file != NULL, "Null parametr\n");
	__asrt(n != NULL, "Null parametr\n");

	__asrt(n->children != NULL, "Null parametr\n");

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
			"Context Try_%s_%ld(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);\n",
			name_of_rule, cur_child + 1);
		// Firstly we will write all the options of this rule to the parser file.
		fprintf(c_file,
			"Context Try_%s_%ld(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)\n{\n",
			name_of_rule, cur_child + 1);

		tabs[n_tabs++] = '\t';

		fprintf(c_file,
			"%s__msg(D_PARSER_WORK, M, \"%s\\n\");\n",
			tabs, name_of_rule);

		fprintf(c_file,
			"%sContext try_ctx = ctx;\n"
			"%sTree %s_%ld_tree = {0};\n",
			tabs, tabs, name_of_rule, cur_child + 1);

		fprintf(c_file,
			"%sAddChild(&%s_%lu_tree, CreateNode(&%s_%lu_tree, sequence + n_tokens - 1));\n"
			"%sContext new_ctx = {0};\n\n",
			tabs, name_of_rule, cur_child + 1,
			name_of_rule, cur_child + 1,
			tabs);

		Node *chain = GetChild(fork, cur_child);
		__tab_incr();
		while (chain->children != NULL) {
			WriteChain(c_file, chain, tabs, cur_child, name_of_rule);
			chain = GetChild(chain, 0);
		}
		/* Write last chain in sequence in particular
		line in rule.*/
		WriteChain(c_file, chain, tabs, cur_child, name_of_rule);
		fprintf(c_file,
			"\tAppendTree(t, &%s_%lu_tree);\n",
			name_of_rule, cur_child + 1);

		__tab_decr();
		__msg(D_FILE_PRINT, M,
					"end of line\n");

		fprintf(c_file,
			"%s__msg(D_PARSER_WORK, M, \"EXACTLY Try_%s_%lu\\n\");\n",
			tabs, name_of_rule, cur_child + 1);

		fprintf(c_file,
			"%sInsertParent(t, CreateNode(t, sequence + n_tokens - 1));\n",
			tabs);

		--n_tabs;
		tabs[n_tabs] = '\0';

		fprintf(c_file, "\treturn try_ctx;\n");
		fprintf(c_file, "}\n\n");
	}

	/* Secondly print prefix of parser
	function that contains local context - if that function
	can be applied to the current place in sequence of token
	this context will commit it and contain local progress
	in parsing sequence.*/
	WritePrefixOfParserFunction(header_file, c_file, name_of_rule);

	__msg(D_PARSER_GENERATING, M,
		"Not tabs:\"%s\"(%lu)\n",
		tabs, n_tabs);

	__msg(D_PARSER_GENERATING, M,
		"Current rule has (%lu) options\n",
		fork->children->size);

	for (uint64_t cur_child = 0; cur_child < fork->children->size; ++cur_child) {
		WriteReferenceToOtherRules(c_file, tabs, cur_child, &n_tabs, name_of_rule);
	}

	/* If all rules can't be applied to the sequence that was wrong rule
	to parse current place in token sequence so we return original contest.*/
	fprintf(c_file, "%s\t__tab_decr();\n", tabs);
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
		"\n\tAppendTree(t, &%s_tree);\n"
		"\treturn new_ctx;\n", name_of_rule);

	fprintf(c_file, "}\n\n");
}


void GenerateCommands(FILE *header_file, FILE *c_file, Node *n, char *curcmd)
{
	__asrt(header_file != NULL, "Null parametr\n");
	__asrt(n           != NULL, "Null parametr\n");
	__asrt(curcmd      != NULL, "Null parametr\n");

	if (n->rule_name) {
		// Check if it's rule root node.
		__msg(D_FILE_PRINT, M,
			"Found rule-node:\"%s\"\n",
			n->token->txt);
		/* If that's rule name we will
		generate function text and print it
		to the parser file.*/
		__tab_incr();
		GenerateCommand(header_file, c_file, n);
		__tab_decr();
	} else {
		if (n->children != NULL) {
			/*If that node is not a rule-root node, maybe it
			contains rule-node in it's children so we will check
			nodes one by one.*/
			for (uint64_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
				__msg(D_FILE_PRINT, M,
					"This node NOT rule:\"%s\"\n",
					n->token->txt);
				__msg(D_FILE_PRINT, M,
					"Start to check childs nodes\n");
				__tab_incr();
				GenerateCommands(header_file, c_file, GetChild(n, cur_child), curcmd);
				__tab_decr();
				__msg(D_FILE_PRINT, M,
					"End of parsing children\n");
			}
		}
	}
}

void WriteObviousCommands(FILE *header_file, FILE *c_file)
{
	fprintf(header_file,
		"#pragma once\n\n"
		"#include <stdlib.h>\n"
		"#include <stdio.h>\n"
		"#include <include/RebeccaCompiler.h>\n\n");

	fprintf(header_file,
		"Context TryToken(Tree *t, Token *sequence, TokenType expected_type, Context ctx, uint64_t n_tokens);\n");

	fprintf(c_file,
		"Context TryToken(Tree *t, Token *sequence, TokenType expected_type, Context ctx, uint64_t n_tokens)\n"
		"{\n"
		"\t__msg(D_PARSER_WORK, M, \"TryToken start t(%%s|idx:%%lu)\\n\", sequence[ctx.cur_token_idx].txt, ctx.cur_token_idx);\n"
		"\tTree try_token_tree = {0};\n"
		"\tAddChild(&try_token_tree, CreateNode(&try_token_tree, sequence + n_tokens - 1));\n"
		"\tif (sequence[ctx.cur_token_idx].type == expected_type) {\n"
		"\t\tAddChild(&try_token_tree, CreateNode(&try_token_tree, sequence + ctx.cur_token_idx));\n"
		"\t\t++ctx.n_parsed;\n"
		"\t\t++ctx.cur_token_idx;\n"
		"\t} else { return ctx; }\n\n"
		"\tParent(&try_token_tree);\n"
		"\tAppendTree(t, &try_token_tree);\n"
		"\t__msg(D_PARSER_WORK, M, \"END of TryToken\\n\");\n"
		"\treturn ctx;\n"
		"}\n\n");
}

Tree *GenerateParserFile(Token *sequence, uint64_t n_tokens)
{
	__asrt(sequence != NULL, "Null parametr\n");

	Tree *t = GenerateParserAst(sequence, n_tokens);
	__msg(D_PARSER_GENERATING, M,
		"Picture of tree was created!\n");

	__spt(D_PARSER_GENERATING);

	__msg(D_NAMETABLE, M,
		"Start of work of nametable\n");
	// Serch start position to write commands.
	char rule_to_write[kTokenMaxLen] = "";
		/* To generate parser's file
		It's necessary to extract all the variables from the tree
		to identify which tokens rules are refer to.*/
	NameTable *table = ScanForNames(t, rule_to_write);
		// Generate picture of created tree.
	DebugTree(t);

	__msg(D_NAMETABLE, M,
		"Returned:%s\n", rule_to_write);

	for (size_t cur_el = 0; cur_el < table->size; ++cur_el) {
		__msg(D_NAMETABLE, M,
			"name[%d]:%s\n", cur_el, table->names[cur_el].txt);
	}

	__spt(D_PARSER_GENERATING);


  // TryToken(&additive_expression_1_tree, sequence, TOKEN_PLUS, try_ctx, n_tokens);
  // Try_additive_expression(&additive_expression_1_tree, sequence, try_ctx, n_tokens);

  for (size_t cur_decl = 0; cur_decl < table->size; ++cur_decl) {
    printf(":%d\n", table->names->parser_type);
  }

	__msg(D_FILE_PRINT, M,
		"Start of generatin file\n");
		/* After extracting all names
		Print all rules with followed structure:
		if we have to parse primary_opr we
		try to parse target token: multiply or devide firstly.
		if we get needed token we return to parent rule.
		If we didn't extract needed token we start descent
		according to the following rule in the hierarchy rule*/
	FILE *header_file = fopen("../src/Parser_GEN_.h", "w");
	FILE *c_file = fopen("../src/Parser_GEN_.c", "w");

	fprintf(c_file,
		"#include <src/Parser_GEN_.h>\n"
		"#include<MchlkrpchLogger/logger.h>\n\n");

	// Adds to file pragma, includes, parse one token function.
	WriteObviousCommands(header_file, c_file);

	__tab_incr();
	GenerateCommands(header_file, c_file, t->root, rule_to_write);
	__tab_decr();

	fclose(header_file);
	fclose(c_file);

	return t;
}

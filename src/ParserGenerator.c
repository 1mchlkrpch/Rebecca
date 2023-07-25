#include <stdint.h>
#include <stdio.h>

#include <include/RebeccaCompiler.h>
#include <MchlkrpchLogger/logger.h>

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
 * 
 */
void ReadGrammarRule(Tree *t, Token *sequence, uint64_t *cur_token_idx)
{
	assert(t             != NULL && "Null param");
	assert(sequence      != NULL && "Null param");
	assert(cur_token_idx != NULL && "Null param");

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

	// Rule's line parsing.
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
			old_current->token.txt);

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
				/* Parse quoted argument: we
				should split quotes and add it's data to new token.*/
				__msg(D_PARSER_GENERATING, M,
					"Add quoted data\n");
				// Skip quote.
				++(*cur_token_idx);
				Node *new_rule_option = CreateNode(t, sequence + *cur_token_idx);
				__msg(D_PARSER_GENERATING, M,
					"data:\"%s\"\n",
					new_rule_option->token.txt);

				AddChild(t, new_rule_option);
				++(*cur_token_idx);
				++(*cur_token_idx);
				__msg(D_PARSER_GENERATING, M,
					"Now current token is:\"%s\"\n",
					(sequence + *cur_token_idx)->txt);
				continue;
			}
			__tab_decr();

			Node *new_rule_option = CreateNode(t, sequence + *cur_token_idx);
			AddChild(t, new_rule_option);
			++(*cur_token_idx);
		}

		t->current = old_current;
	}
	// Stop parse lines.
	__tab_decr();

	t->current = primal_node;
	// Returns old current node in tree.
	__msg(D_PARSER_GENERATING, M,
		"Now current in tree is:\"%s\"\n",
		t->current->token.txt);
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
	// DEBUG_MSG(D_TREE,      W, "created\n");
	assert(t             != NULL && "Null param");
	assert(sequence      != NULL && "Null param");
	assert(cur_token_idx != NULL && "Null param");

	// Insert 'TOKEN_EQ' token in tree.
	Node *equal_node = CreateNode(t, sequence + *cur_token_idx);
	InsertParent(t, equal_node);
	++(*cur_token_idx);

	/* Next token indicates to
	identifier = (>)'SYMBOL' or identifier = (>)"CONSTANT"*/ 
	Token *next = sequence + *cur_token_idx;

	if (next->type == TOKEN_SINGLE_QUOTE ||
		  next->type == TOKEN_DOUBLE_QUOTE) {
		// Skip comma token and insert data token.
		++(*cur_token_idx);

		Node *old_current = t->current;
		while (sequence[*cur_token_idx].type != TOKEN_SINGLE_QUOTE &&
		  		 sequence[*cur_token_idx].type != TOKEN_DOUBLE_QUOTE) {
			Node *new_child = CreateNode(t, sequence + *cur_token_idx);
			AddChild(t, new_child);
			++(*cur_token_idx);
		}

		t->current = old_current;
	}
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
 */
Tree *GenerateParserAst(Token *sequence, uint64_t n_tokens)
{
	assert(sequence != NULL && "Null param");

	__msg(D_PARSER_GENERATING, M, "Start of generating parser's AST\n");

	// Tree t = {0};
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
			__msg(D_PARSER_GENERATING, M,
				"Token with data:\"%s\" was added\n",
				new_node->token.txt);

			/* Set this flag as true to realize that
			construction NAME + '=' + ... can be grammar rule or definition.*/
			previous_token_is_name = true;
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
					__msg(D_PARSER_GENERATING, M,
						"Previous name followed by rule\n");

					ReadGrammarRule(t, sequence, &cur_token_idx);
					/* Move to parent to
					add new rules and definitions to the tree.*/
					Parent(t);
					__msg(D_PARSER_GENERATING, M,
						"Current parent after \'ReadGrammarRule\' is:\"%s\"\n",
						t->current->token.txt);
					break;
				}
				// Text definition.
				case TOKEN_EQ: {
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

void GenerateParserFile(Token *sequence, uint64_t n_tokens)
{
	Tree *t = GenerateParserAst(sequence, n_tokens);
	DebugTree(t);
	__msg(D_PARSER_GENERATING, M,
		"Picture of tree was created!\n");
}

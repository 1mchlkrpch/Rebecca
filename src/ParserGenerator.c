#include <stdint.h>
#include <stdio.h>

#include <include/RebeccaCompiler.h>

void ReadGrammarRule(Tree *t, Token *sequence, uint64_t *cur_token_idx)
{
	assert(t             != NULL && "Null param");
	assert(sequence      != NULL && "Null param");
	assert(cur_token_idx != NULL && "Null param");

	Node *primal_node = t->current;
	Node *or_node = CreateNode(t, sequence + *cur_token_idx);
	AddChild(t, or_node);
	++(*cur_token_idx);

	while (sequence[*cur_token_idx].type != TOKEN_SEMICOLON) {
		if (sequence[*cur_token_idx].type == TOKEN_PIPE) {
			++(*cur_token_idx);
			continue;
		}

		Node *old_current = t->current;
		while (true) {
			if (sequence[*cur_token_idx].type == TOKEN_PIPE ||
			    sequence[*cur_token_idx].type == TOKEN_SEMICOLON) {
				break;
			}

			if (sequence[*cur_token_idx].type == TOKEN_SINGLE_QUOTE ||
				  sequence[*cur_token_idx].type == TOKEN_DOUBLE_QUOTE) {
				++(*cur_token_idx);
				Node *new_rule_option = CreateNode(t, sequence + *cur_token_idx);
				AddChild(t, new_rule_option);
				++(*cur_token_idx);
				++(*cur_token_idx);
				continue;
			}

			Node *new_rule_option = CreateNode(t, sequence + *cur_token_idx);
			AddChild(t, new_rule_option);
			++(*cur_token_idx);
		}

		t->current = old_current;
	}

	t->current = primal_node;
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

		Node *new_child = CreateNode(t, sequence + *cur_token_idx);
		AddChild(t, new_child);
		Parent(t);

		++(*cur_token_idx);
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
void GenerateParserFile(Token *sequence, uint64_t n_tokens)
{
	assert(sequence != NULL && "Null param");

	Tree t = {0};
	bool previous_token_is_name = false;

	Node *common_node = CreateNode(&t, sequence + n_tokens - 1);
	AddChild(&t, common_node);

	size_t cur_token_idx = 0;
	for (; cur_token_idx < n_tokens; ++cur_token_idx) {
		if (sequence[cur_token_idx].type == TOKEN_NAME) {			
			Node *new_node = CreateNode(&t, sequence + cur_token_idx);
			AddChild(&t, new_node);

			previous_token_is_name = true;
			continue;
		}

		if (previous_token_is_name) {
			previous_token_is_name = false;
			/* Name token may be inheritated by
			text definition (by '='-symbol) or grammar rule (':'-symbol*/
			switch (sequence[cur_token_idx].type) {
				// Grammar rule.
				case TOKEN_COLON: {
					ReadGrammarRule(&t, sequence, &cur_token_idx);
					Parent(&t);
					break;
				}
				// Text definition.
				case TOKEN_EQ: {
					ReadDefinition(&t, sequence, &cur_token_idx);
					Parent(&t);
					break;
				}

				default:{
					continue;
				}
			}
		}
	}

	DebugTree(&t);
}

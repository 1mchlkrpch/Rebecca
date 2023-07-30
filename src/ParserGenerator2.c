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

Tree *GenerateParserAst(Tree *parser_tree, Tree *tokenizer_tree, Token *s, uint64_t n_tokens)
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

void GenerateParserFile(Token *s, uint64_t n_tokens)
{
	__asrt(s != NULL, "Null param");

	Tree *tokenizer_tree = TreeCtor(TOKENIZER_TREE);
	Tree *parser_tree    = TreeCtor(PARSER_TREE);

	GenerateParserAst(parser_tree, tokenizer_tree, s, n_tokens);
	DebugTree(parser_tree);
	DebugTree(tokenizer_tree);
	/* After tree generation
	generator should generate tokenizer file.
	it will contain */
}
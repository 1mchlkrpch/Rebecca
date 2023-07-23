#include <stdint.h>
#include <stdio.h>

#include <include/RebeccaCompiler.h>

void FillEmptyStr(char **dest, const char *src)
{
	assert(src  != NULL && "Null param");

	(*dest) = (char *)calloc(kTokenMaxLen, sizeof(char));
	assert(dest != NULL && "Null calloc allocation");

	strcpy(*dest, src);
}

void ReadGrammarRule(Tree *t, Token *sequence, uint64_t *cur_token_idx)
{
	assert(t             != NULL && "Null param");
	assert(sequence      != NULL && "Null param");
	assert(cur_token_idx != NULL && "Null param");
}

void ReadDefinition(Tree *t, Token *sequence, uint64_t *cur_token_idx)
{
	assert(t             != NULL && "Null param");
	assert(sequence      != NULL && "Null param");
	assert(cur_token_idx != NULL && "Null param");

	Node *equal_node = CreateNode(t, sequence + *cur_token_idx);
	// printf("%s->%s\n", t->root->token.txt, GetChild(t->root, 0)->token.txt);
	InsertParent(t, equal_node);
	// printf("rot txt:%s\n", t->root->token.txt);
	// if (strcmp(sequence[*cur_token_idx - 1].txt, "lol") == 0) {
	// 	printf("crt IN READ:%s|%d\n", t->root->token.txt, t->root->id);
	// 	DebugTree(t);
	// 	exit(0);
	// }
	++(*cur_token_idx);
	// printf("%s->%s->%s\n", t->root->token.txt, GetChild(t->root, 0)->token.txt, GetChild(GetChild(t->root, 0), 0)->token.txt);


	// DebugTree(t);
	// exit(0);

	/* Next token indicates to
	identifier = (>)'SYMBOL' or identifier = (>)"CONSTANT"*/ 
	Token *next = sequence + *cur_token_idx;
	
	// printf("cur in tree:%s\n", t->current->token.txt);
	if (next->type == TOKEN_SINGLE_QUOTE ||
		  next->type == TOKEN_DOUBLE_QUOTE) {
		++(*cur_token_idx);

		Node *new_child = CreateNode(t, sequence + *cur_token_idx);

		AddChild(t, new_child);
		// printf("d\n");
		// printf("%s->%s->%s|%s\n", t->root->token.txt, GetChild(t->root, 0)->token.txt, GetChild(GetChild(t->root, 0), 0)->token.txt, GetChild(GetChild(t->root, 0), 1)->token.txt);
		Parent(t);
		// printf("crrent in tree:%s\n", t->current->token.txt);

		++(*cur_token_idx);
	}

		// printf("h?\n");
}

Node *CreateNode(Tree *t, Token *token)
{
	Node *new_node = NodeCtor();
	// new_node->token = *token;
	FillEmptyStr(&new_node->token.txt, token->txt);
	new_node->token.type = token->type;
	new_node->id = t->size;

	++t->size;

	return new_node;
}

void GenerateParserFile(Token *sequence, uint64_t n_tokens)
{
	assert(sequence != NULL && "Null param");

	Tree t = {0};

	uint64_t last_name_idx = 0;
	bool previous_token_is_name = false;

	Node *common_node = CreateNode(&t, sequence + n_tokens - 1);
	AddChild(&t, common_node);

	size_t cur_token_idx = 0;
	for (; cur_token_idx < n_tokens; ++cur_token_idx) {
		if (sequence[cur_token_idx].type == TOKEN_NAME) {
			printf("name:%s\n", sequence[cur_token_idx].txt);

			if (strcmp(sequence[cur_token_idx].txt, "lol") == 0) {
				printf("crrent in tree:%s\n", t.current->token.txt);
			}
			
			Node *new_node = CreateNode(&t, sequence + cur_token_idx);
			AddChild(&t, new_node);

			// if (strcmp(sequence[cur_token_idx].txt, "lol") == 0) {
			// 	printf("crrent in tree:%s\n", t.current->token.txt);
			// 	DebugTree(&t);
			// 	exit(0);
			// }
			// printf("crrent in tree:%s\n", GetChild(t.current->parent, 0)->token.txt);

			// printf("sz:%zu", sizeof(Node*));
			// printf("newnode?%s\n", new_node->token.txt);
			// printf("child?%s\n", GetChild(new_node, 0)->token.txt);

			// if (strcmp(sequence[cur_token_idx].txt, "lol") == 0) {
			// 	DebugTree(&t);
			// 	exit(0);
			// }

			previous_token_is_name = true;
			last_name_idx = cur_token_idx;
			continue;
		}

		if (previous_token_is_name) {
			// Turn off flag and check next token
			previous_token_is_name = false;
			/* Name token may be inheritated by
			text definition (by '='-symbol) or grammar rule (':'-symbol*/
			switch (sequence[cur_token_idx].type) {
				// Grammar rule.
				case TOKEN_COLON: {
					ReadGrammarRule(&t, sequence, &cur_token_idx);
					break;
				}
				// Text definition.
				case TOKEN_EQ: {
					// printf("def again\n");
					// if (strcmp(sequence[cur_token_idx].txt, "lol") == 0) {
					// 	printf("crrent in tree:%s\n", t.current->token.txt);
					// 	DebugTree(&t);
					// 	exit(0);
					// }
					ReadDefinition(&t, sequence, &cur_token_idx);
					Parent(&t);
					// printf("d?\n");
					// InsertParent(&t, NodeCtor());
					// printf("crrent in tree:%s\n", t.current->token.txt);
					// DebugTree(&t);
					// exit(0);
					// printf("current token after read def:%s\n", sequence[cur_token_idx].txt);
					break;
				}

				default:{
					continue;
				}
			}
		}
	}

	DebugTree(&t);

	// printf("root: %s\nchild: %s\n", t.root->token.txt, GetChild(t.root, 0)->token.txt);
}

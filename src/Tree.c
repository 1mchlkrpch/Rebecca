#include <include/RebeccaCompiler.h>

Node *NodeCtor()
{
  Node *n = (Node *)calloc(1, sizeof(Node));
  assert(n != NULL && "Null calloc allocation");

  return n;
}

Node *AddChild(Tree *t, Node *new_child)
{
	assert(t != NULL && "Null param");
	assert(new_child != NULL && "Null param");
	assert(new_child->token.txt != NULL && "Null param");

	if (t->root == NULL) {
		t->root    = new_child;
		t->current = t->root;
	} else {
		if (t->current->children == NULL) {
			// printf("add new array\n");
			// printf("zu:%zu\n", sizeof(Node*));
			t->current->children = ArrayCtor(sizeof(Node*));
			// assert(t->root->children != NULL);
		}

		// if (t->current == t->root) {
		// 	printf("the same\n");
		// }

		Array *a = t->current->children;
		if (a->size == a->capacity) {
			ArrayChangeCapacity(a, a->capacity << 1);
		}
		// printf("array a:sz:%ld\n", a->size);
		void *ptr = (char *)a->data + a->size * a->element_size;

		memcpy(ptr, &new_child, a->element_size);

		// printf("after memcpy:%s|from:%s\n",
		// 		GetChild(t->current, 0)->token.txt,
		// 		((Node*)new_child)->token.txt);

		GetChild(t->current, a->size)->parent = t->current;
		// printf("after memcpy:%s\n", ((Node*)t->root->children->data)->token.txt);
		++a->size;

		// printf("tok of new child:%s\n", new_child->token.txt);
		// ArrayAdd(t->current->children, new_child);
		// // printf("getchild after Array add:%s\n", GetChild(t->current, 0)->token.txt);
		// printf("getchild after Array add:%s\n", ((Node*)t->current->children->data)->token.txt);
		// new_child->parent = (struct Node *)t->current;

		t->current = new_child;
	}

	return t->current;
}

// Node *parent = t->current->parent;

// t->current = parent;
// uint64_t find_children_idx = 0;
// for (uint64_t child_idx = 0; child_idx < parent->children->size; ++child_idx) {
// 	Node *cur_parent_child = GetChild(parent, child_idx);
// 	if (cur_parent_child->id == n->id) {
// 		find_children_idx = child_idx;
// 		// printf("found idx:%ld\n", );
// 		break;
// 	}
// }

// // AddChild(t, old_current);
// printf("idx:%d\n", find_children_idx);
// void *ptr = (char *)parent->children->data + find_children_idx * parent->children->element_size;
// // memcpy((char *)parent->children->data + find_children_idx * sizeof(Node*), &old_current, sizeof(Node*));
// memcpy(&parent->children->data, &n, sizeof(Node*));
// printf("det:%s\n", (*(Node**)parent->children->data)->token.txt);
// n->parent = parent;

void InsertParent(Tree *t, Node *n)
{
	Node *old_current = t->current;

	printf("WTF\n");
	// assert(t->root->children != NULL);

	if (t->current->parent != NULL) {
		Node *parent = t->current->parent;
		printf("replace ptr sz:%d|%s\n", parent->children->size, parent->token.txt);
		printf("rootsz:%d|%s\n", t->root->children->size, t->root->token.txt);
		memcpy((Node **)parent->children->data + parent->children->size - 1, &n, sizeof(Node*));
			
		// ((Node**)(parent->children->data))[parent->children->size - 1] = n;
		n->parent = parent;
		// if (t->root->children->size == 2) {
		// 	printf("predef\n");
		// 	printf("det:%s\n", GetChild(t->root, 1)->token.txt);
		// 	exit(0);
		// }
		printf("?:%d\n", t->root->children->size);
	}

	t->current = n;
	AddChild(t, old_current);
	Parent(t);
	if (t->root->children->size == 2) {
		printf("%s->%s->%s\n",
				t->root->token.txt,
				GetChild(t->root, 1)->token.txt,
				GetChild(GetChild(t->root, 1), 0)->token.txt);
	}

	// printf("in insertParent:%s\n", t->current->token.txt);
	old_current->parent = (struct Node *)n;

	if (old_current == t->root) {
		t->root = n;
	}
	// printf("debug parent txt:%s\n", t->root->token.txt);
}

static Parser *ParserCtor(Token *sequence)
{
	assert(sequence != NULL && "Null param");

	Parser *parser = (Parser *)calloc(1, sizeof(Parser));
	assert(parser != NULL && "Null calloc allocation");

	parser->sequence = sequence;
	parser->current_token = parser->sequence;

	parser->tree = (Tree *)calloc(1, sizeof(Tree));
	assert(parser->tree != NULL && "Null calloc allocation");

	return parser;
}

void PrintNode(FILE *f, Node *n)
{
	assert(f != NULL && "Null param");
	assert(n != NULL && "Null param");

	printf("debug:n:%d, %s\n", n->id, n->token.txt);

	fprintf(f, "\tn%lu [shape=diamond label=<\n"
			"\t\t<table border=\"0\">\n"
			  "\t\t\t<tr><td colspan=\"1\" bgcolor=\"Peru\">%s</td><td>%lu</td></tr>\n"
			  "\t\t\t<tr><td colspan=\"2\" bgcolor=\"Peru\">%s</td></tr>\n"
			"\t\t</table>\n"
		"\t>]\n", n->id, TranslateTokenType(n->token.type), n->id, n->token.txt);

	fprintf(f, "\n");

	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			PrintNode(f, GetChild(n, cur_child));
		}
	}
}

void ConnectNode(FILE *f, Node *n)
{
	assert(f != NULL && "Null param");
	assert(n != NULL && "Null param");

	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			fprintf(f, "\tn%ld -> n%ld\n", n->id, GetChild(n, cur_child)->id);
		}
	}

	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			ConnectNode(f, GetChild(n, cur_child));
		}
	}
}

void DebugTree(Tree *t)
{
	assert(t != NULL && "Null param");

	FILE *f = fopen("../graph.dot", "w");
	assert(f != NULL && "Reading file error");

	// printf("d?\n");
	// printf("crt IN READ:%s\n", t->root->token.txt);
	fprintf(f, "digraph G{\n");
	PrintNode(f, t->root);
	ConnectNode(f, t->root);
	fprintf(f, "}\n");

	fclose(f);

	system("dot -Tpng ../graph.dot -o ../graph.png");
}

void Parent(Tree *t)
{ t->current = (Node *)t->current->parent; }

Node *GetChild(Node *n, uint64_t idx)
{ return (*(Node **)((char *)n->children->data + idx * sizeof(Node*))); }




// void FillCurrent(Tree *t, Token *token)
// {
// 	assert(t     != NULL && "Null param");
// 	assert(token != NULL && "Null param");

// 	t->current->token = *token;
// }

// static void ParseIdentifier(Parser *parser)
// {
// 	assert(parser != NULL && "Null param");
// }

// Parser *BuildAst(Token *sequence, uint64_t n_tokens)
// {
// 	assert(sequence != NULL && "nullptr param");
// 	(void)n_tokens;

// 	Parser *parser = ParserCtor(sequence);
// 	assert(parser != NULL && "Null pointer to parser");

// 	// Add tokens
// 	// Add png debugger
// 	// syntax analysis
// 	ParseIdentifier(parser);

// 	printf("token txt:%s\n", parser->tree->current->token.txt);

// 	return parser;
// }

#include <include/RebeccaCompiler.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

/**
 * @brief Allocates new node.
 * @returns   New allocated node.
 */
Node *NodeCtor()
{
  Node *n = (Node *)calloc(1, sizeof(Node));
  assert(n != NULL && "Null calloc allocation");

  return n;
}

/**
 * @brief Adds new child to 't->current'-node with new_child node.
 * Also makes 't-current'-node equal to inserted child.
 * 
 * @param t           Tree to append
 * @param new_child   New node to insert in 't'.
 * @returns           New 't->current'-node.
 */
Node *AddChild(Tree *t, Node *new_child)
{
	assert(t                    != NULL && "Null param");
	assert(new_child            != NULL && "Null param");
	assert(new_child->token.txt != NULL && "Null param");

	if (t->root == NULL) {
		t->root    = new_child;
		t->current = t->root;
	} else {
		if (t->current->children == NULL) {
			t->current->children = ArrayCtor(sizeof(Node*));
		}

		ArrayAdd(t->current->children, new_child);
		GetChild(t->current, t->current->children->size - 1)->parent = (struct Node *)t->current;

		t->current = new_child;
	}

	return t->current;
}

/**
 * @brief Insert's parent node 'n' as parent
 * for 't->current'-node.
 * 
 * @param t   Tree to append.
 * @param n   New parent node.
 */
void InsertParent(Tree *t, Node *n)
{
	Node *old_current = t->current;

	if (t->current->parent != NULL) {
		Node *parent = t->current->parent;
		// TODO: fix always last place in array correction.
		// sometimes we want to change parent for not last inserted node.
		memcpy((Node **)parent->children->data + parent->children->size - 1, &n, sizeof(Node*));
		n->parent = parent;
	}

	t->current = n;
	AddChild(t, old_current);
	Parent(t);

	old_current->parent = (struct Node *)n;

	if (old_current == t->root) {
		t->root = n;
	}
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

/**
 * @brief Fills .dot file 'f' with data of particular node.
 * Works recursively.
 * 
 * @param f   File to fill with data of node.
 * @param n   Node to write to file 'f'.
 */
void PrintNode(FILE *f, Node *n)
{
	assert(f != NULL && "Null param");
	assert(n != NULL && "Null param");

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

/**
 * @brief Fills .dot file 'f' with connections
 * of particular node with it's children.
 * Works recursively.
 * 
 * @param f   File to fill with data of node.
 * @param n   Node to write to file 'f'.
 */
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

/**
 * @brief Draws tree 't' with graphviz software.
 * @param t   Tree to draw.
 */
void DebugTree(Tree *t)
{
	assert(t != NULL && "Null param");

	FILE *f = fopen("../graph.dot", "w");
	assert(f != NULL && "Reading file error");

	fprintf(f, "digraph G{\n");
	fprintf(f, "\tgraph [ dpi = 50 ];\n");
	PrintNode(f, t->root);
	ConnectNode(f, t->root);
	fprintf(f, "}\n");
	fclose(f);

	system("dot -Tpng ../graph.dot -o ../graph.png");
}

/**
 * @brief Changes 't->current'-node to 't->current->parent'-node.
 * @param t   Tree to correct.
 */
inline __attribute__((always_inline))
void Parent(Tree *t)
{ t->current = (Node *)t->current->parent; }

/**
 * @brief Gets pointer to particular child
 * of node 'n' with index 'idx' in 'n''s array of children.
 * 
 * @param n     Parent node.
 * @param idx   Index of child to get.
 */
inline __attribute__((always_inline))
Node *GetChild(Node *n, uint64_t idx)
{ return (*(Node **)((char *)n->children->data + idx * sizeof(Node*))); }

/**
 * @brief Creates node with new unique index
 * and adds it to tree 't'.
 * Gathers data from token 'token'.
 * Also increments 't->size'
 * 
 * @param t       Tree to generate new unique 'id' of new node.
 * @param token   Token to gather data.
 * @returns       New generated node.
 */
Node *CreateNode(Tree *t, Token *token)
{
	Node *new_node = NodeCtor();

	FillEmptyStr(&new_node->token.txt, token->txt);
	new_node->token.type = token->type;
	new_node->id = t->size;

	++t->size;

	return new_node;
}

#pragma GCC diagnostic pop

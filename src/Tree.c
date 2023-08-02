#include <include/RebeccaGenerator.h>
#include <MchlkrpchLogger/logger.h>

/**
 * @brief Changes 't->current'-node to 't->current->parent'-node.
 * @param t   Tree to correct.
 */
inline __attribute__((always_inline))
void Parent(Tree *t)
{ t->current = (Node *)t->current->parent; }

TokenType GetType(Node *n)
{ return n->token->type; }

char *GetTxt(Node *n)
{ return n->token->txt; }

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
 * @brief Get's better style of
 * graph of AST tree by give each node
 * it's own shape.
 * 
 * @param t   Type of incoming node from tree.
 * @returns   const char shape-name from type.
 */
static const char *BorderFmt(TokenType t)
{
	switch (t) {
		case TOKEN_COLON:
		case TOKEN_SEMICOLON:
		case TOKEN_DOUBLE_QUOTE:
		case TOKEN_SINGLE_QUOTE:
		case TOKEN_EOF:
		case TOKEN_EQ:   { return "none"; }
		case TOKEN_NAME: { return "rectangle"; }

		default: {
			return 	"diamond";
		}
	}
}

static const char *BorderColor(PrsrNdType type)
{
	switch (type) {
		case            VAR_NAME: { return "yellow"; }
		case           RULE_NAME: { return "cyan"; }
		case RULE_NAME_REFERENCE: { return "red"; }
		case  VAR_NAME_REFERENCE: { return "green"; }

		default: {
			return "black";
		}
	}
}

/**
 * @brief Fills .dot file 'f' with data of particular node.
 * Works recursively.
 * 
 * @param f   File to fill with data of node.
 * @param n   Node to write to file 'f'.
 */
static void PrintNode(FILE *f, Node *n)
{
	assert(f != NULL && "Null param");
	assert(n != NULL && "Null param");

	fprintf(f, NODE_FMT,
			n->id,
			BorderFmt(GetType(n)),
			BorderColor(n->token->parser_type),
			GetTxt(n),
			TranslateTokenType(GetType(n)));

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
static void ConnectNode(FILE *f, Node *n)
{
	assert(f != NULL && "Null param");
	assert(n != NULL && "Null param");

	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			fprintf(f,
					"\tn%ld -> n%ld\n",
					n->id,
					GetChild(n, cur_child)->id);
		}
	}

	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			ConnectNode(f, GetChild(n, cur_child));
		}
	}
}

// Constructors -----------------------------------

Node *CreateNodeByType(Tree *t, TokenType type)
{
	Node *new_node = NodeCtor();

	FillEmptyStr(&new_node->token->txt, TranslateTokenType(type));
	new_node->token->type = type;
	new_node->id = (uint64_t)new_node;

	++t->size;

	return new_node;
}

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

	FillEmptyStr(&new_node->token->txt, token->txt);

	new_node->token->type = token->type;
	new_node->id = ((uint64_t)new_node);

	++t->size;

	return new_node;
}

/**
 * @brief Allocates new node.
 * @returns   New allocated node.
 */
Node *NodeCtor()
{
  Node *n = (Node*)calloc(1, sizeof(Node));
  assert(n != NULL && "Null calloc allocation");

  n->token = (Token *)calloc(1, sizeof(Token));
  assert(n->token != NULL && "Null calloc allocation");

  return n;
}

Tree *TreeCtor(TokenType type)
{
	Tree *new_tree = (Tree*)calloc(1, sizeof(Tree));
	assert(new_tree != NULL && "Null calloc allocation");

	AddChild(new_tree, CreateNodeByType(new_tree, type));

	return new_tree;
}

// Common functions -------------------------------

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
	assert(t                     != NULL && "Null param");
	assert(new_child             != NULL && "Null param");

	assert(GetTxt(new_child)     != NULL && "Null param");

	if (t->current == NULL) {
		t->root    = new_child;
		t->current = t->root;
	} else {
		if (t->current->children == NULL) {
			t->current->children = ArrayCtor(sizeof(Node*));
		}

		ArrayAdd(t->current->children, new_child);
		GetChild(t->current, t->current->children->size - 1)->parent = (Node*)t->current;

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
	assert(t != NULL && "Null param");
	assert(n != NULL && "Null param");

	Node *old_current = t->current;

	if (t->current->parent != NULL) {
		Node *parent = t->current->parent;

		memcpy((Node**)parent->children->data + parent->children->size - 1, &n, sizeof(Node*));
		n->parent = parent;
	}

	t->current = n;
	AddChild(t, old_current);

	old_current->parent = (Node*)n;
	Parent(t);

	if (old_current == t->root) {
		t->root = n;
	}
}

void AppendTree(Tree *first, Tree *second)
{
	assert(first  != NULL && "Null param");
	assert(second != NULL && "Null param");

	Node  *second_cur = second->current;
	Node **children   = (Node**)calloc(second_cur->children->size, sizeof(Node*));

	for (size_t cur_child = 0; cur_child < second_cur->children->size; ++cur_child) {
		Node *child = GetChild(second_cur, cur_child);
		memcpy(children, &child, sizeof(Node*));
	}

	for (size_t cur_child = 0; cur_child < second_cur->children->size; ++cur_child) {
		AddChild(first, GetChild(second_cur, cur_child));
		Parent(first);

		GetChild(first->current, cur_child)->parent = first->current;
	}

	second->current =  first->current;
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
	fprintf(f, "\tgraph [dpi=50];\n\n");
	PrintNode(f, t->root);
	fprintf(f, "\n");
	ConnectNode(f, t->root);
	fprintf(f, "}\n");
	fclose(f);

	system("dot -Tpng ../graph.dot -o ../graph.png");
}

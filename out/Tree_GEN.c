#include <Tokenizer_GEN.h>

#include <MchlkrpchLogger/logger.h>

GEN_Node *GEN_NodeCtor()
{
	GEN_Node *n = (GEN_Node *)calloc(1, sizeof(GEN_Node));
	n->token = (GEN_Token *)calloc(1, sizeof(GEN_Token));
	return n;
}

GEN_Tree *GEN_TreeCtor(GEN_TokenType type)
{
	GEN_Tree *new_tree = (GEN_Tree *)calloc(1, sizeof(GEN_Tree));
	__asrt(new_tree != NULL, "Null calloc allocation");
	GEN_AddChild(new_tree, GEN_CreateNodeByType(new_tree, type));
	return new_tree;
}

GEN_Node *GEN_AddChild(GEN_Tree *t, GEN_Node *new_child)
{
	if (t->current == NULL) {
		t->root    = new_child;
		t->current = t->root;
		} else {
		if (t->current->children == NULL) {
			t->current->children = GEN_ArrayCtor(sizeof(GEN_Node*));
		}
		GEN_ArrayAdd(t->current->children, new_child);
		GEN_GetChild(t->current, t->current->children->size - 1)->parent = (struct GEN_Node *)t->current;
		t->current = new_child;
	}
	return t->current;
}

void GEN_InsertParent(GEN_Tree *t, GEN_Node *n)
{
	GEN_Node *old_current = t->current;
	if (t->current->parent != NULL) {
		GEN_Node *parent = t->current->parent;
		memcpy((GEN_Node **)parent->children->data + parent->children->size - 1, &n, sizeof(GEN_Node*));
		n->parent = parent;
	}
	t->current = n;
	GEN_AddChild(t, old_current);
	old_current->parent = (struct GEN_Node *)n;
	GEN_Parent(t);
	if	(old_current == t->root) {
		t->root = n;
	}
}

const char *GEN_CellBordersFormat(GEN_TokenType t)
{
	switch (t) {
		default: {
			return 	"diamond";
		}
	}
}

const char *GEN_CheckIfRuleName(GEN_PrsrNdType type)
{
	switch (type) {
		case            GEN_VAR_NAME: { return "yellow"; }
		case           GEN_RULE_NAME: { return "cyan"; }
		case GEN_RULE_NAME_REFERENCE: { return "red"; }
		case  GEN_VAR_NAME_REFERENCE: { return "green"; }
		default: {return "black"; }
	}
}

GEN_TokenType GEN_Ttype(GEN_Node *n)
{ return n->token->type; }

void GEN_PrintNode(FILE *f, GEN_Node *n)
{
	assert(f != NULL && "Null param");
	assert(n != NULL && "Null param");
	if (strcmp(n->token->txt, GEN_TranslateTokenType(n->token->type)) == 0) {
		fprintf(f, NODE_FMT,
		n->id,
		GEN_CellBordersFormat(n->token->type),
		GEN_CheckIfRuleName(n->token->parser_type),
		n->token->txt,
		"");
	} else {
		fprintf(f, NODE_FMT,
			n->id,
			GEN_CellBordersFormat(n->token->type),
			GEN_CheckIfRuleName(n->token->parser_type),
			n->token->txt,
			GEN_TranslateTokenType(n->token->type));
	}
	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			GEN_PrintNode(f, GEN_GetChild(n, cur_child));
		}
	}
}

void GEN_ConnectNode(FILE *f, GEN_Node *n)
{
	assert(f != NULL && "Null param");
	assert(n != NULL && "Null param");
	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			fprintf(f, "\tn%ld -> n%ld\n", n->id, GEN_GetChild(n, cur_child)->id);
		}
	}
	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			GEN_ConnectNode(f, GEN_GetChild(n, cur_child));
		}
	}
}

void GEN_DebugTree(GEN_Tree *t)
{
	assert(t != NULL && "Null param");
	FILE *f = fopen("../graph.dot", "w");
	assert(f != NULL && "Reading file error");
	fprintf(f, "digraph G{\n");
	fprintf(f, "	graph [dpi=50];\n\n");
	GEN_PrintNode(f, t->root);
	fprintf(f, "\n");
	GEN_ConnectNode(f, t->root);
	fprintf(f, "}\n");
	fclose(f);
	system("dot -Tpng ../graph.dot -o ../graph.png");
}

void GEN_Parent(GEN_Tree *t)
{ t->current = (GEN_Node *)t->current->parent; }

inline __attribute__((always_inline))
GEN_Node *GEN_GetChild(GEN_Node *n, uint64_t idx)
{ return (*(GEN_Node **)((char *)n->children->data + idx * sizeof(GEN_Node*))); }

void GEN_AppendTree(GEN_Tree *first, GEN_Tree *second)
{
	GEN_Node  *second_cur = second->current;
	GEN_Node **children   = (GEN_Node **)calloc(second_cur->children->size, sizeof(GEN_Node*));
	for (size_t cur_child = 0; cur_child < second_cur->children->size; ++cur_child) {
		GEN_Node *child = GEN_GetChild(second_cur, cur_child);
		memcpy(children, &child, sizeof(GEN_Node*));
	}
	for (size_t cur_child = 0; cur_child < second_cur->children->size; ++cur_child) {
		GEN_AddChild(first, GEN_GetChild(second_cur, cur_child));
		GEN_Parent(first);
		GEN_GetChild(first->current, cur_child)->parent = first->current;
	}
	second->current =  first->current;
}

GEN_Node *GEN_CreateNodeByType(GEN_Tree *t, GEN_TokenType type)
{
	GEN_Node *new_node = GEN_NodeCtor();
	GEN_FillEmptyStr(&new_node->token->txt, GEN_TranslateTokenType(type));
	new_node->token->type = type;
	new_node->id = (uint64_t)new_node;
	++t->size;
	return new_node;
}

GEN_Node *GEN_CreateNode(GEN_Tree *t, GEN_Token *token)
{
	GEN_Node *new_node = GEN_NodeCtor();
	GEN_FillEmptyStr(&new_node->token->txt, token->txt);
	new_node->token->type = token->type;
	new_node->id = ((uint64_t)new_node);
	++t->size;
	return new_node;
}

GEN_Array *GEN_ArrayCtor(uint64_t el_sz)
{
	GEN_Array *a = (GEN_Array *)calloc(1, sizeof(GEN_Array));
	assert(a != NULL && "Null calloc allocation");
	a->size         = 0;
	a->capacity     = 1;
	a->element_size = el_sz;
	a->data = calloc(1, el_sz * sizeof(char));
	assert(a->data != NULL && "Null calloc allocation");
	return a;
}

void GEN_FillEmptyStr(char **dest, const char *src)
{
	assert(src  != NULL && "Null param");
	(*dest) = (char *)calloc(kTokenMaxLen, sizeof(char));
	assert(dest != NULL && "Null calloc allocation");
	strcpy(*dest, src);
}

void GEN_ArrayChangeCapacity(GEN_Array *a, uint64_t new_capacity)
{
	assert(a != NULL && "Null param");
	a->data = realloc(a->data, a->element_size * new_capacity * sizeof(char));
	assert(a->data != NULL && "Null reallocation");
	a->capacity = new_capacity;
}

void GEN_ArrayAdd(GEN_Array *a, void *new_element)
{
	if (a->size == a->capacity) {
		GEN_ArrayChangeCapacity(a, a->capacity << 1);
	}
	void *ptr = (char *)a->data + a->size * a->element_size;
	memcpy(ptr, &new_element, a->element_size);
	++a->size;
}

void GEN_CompressTree(GEN_Tree *t, GEN_Node *n)
{
   t->current = n;
	int n_changes = 0;
   while(true) {
		if (n->children != NULL) {
			GEN_Node *save_cur = n;
			for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
				GEN_Node *child = GEN_GetChild(n, cur_child);
				if (child->children != NULL && child->children->size == 1) {
					GEN_Node *grandson = GEN_GetChild(child, 0);
					grandson->parent = n;
					
					++n_changes;
					memcpy((GEN_Node**)n->children->data + cur_child, &grandson, sizeof(GEN_Node*));
				}
			}
		}
		if (n_changes == 0) {
			break;
		}
		n_changes = 0;
	}
	if (n->children != NULL) {
		for (size_t cur_child = 0; cur_child < n->children->size; ++cur_child) {
			GEN_CompressTree(t, GEN_GetChild(n, cur_child));
		}
	}
}

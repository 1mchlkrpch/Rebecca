#pragma once

typedef struct GEN_Node
{
	// Children of particular GEN_node.
	Array           *children;
	// Token of current cell in AST.
	GEN_Token       *token;
	// Numbered-id for each GEN_node.
	uint64_t     id;
	// Parent GEN_node for current one.
	struct GEN_Node *parent;
	// bool         rule_name;
	// Type of GEN_node in parser generating.
} GEN_Node;
typedef struct
{
	GEN_Node   *root;
	GEN_Node   *current;
	size_t      size;
	const char *data;
} GEN_Tree;
GEN_Node *GEN_NodeCtor();

GEN_Tree *GEN_TreeCtor(TokenType type);

GEN_Node *GEN_AddChild(GEN_Tree *t, GEN_Node *new_child);

void GEN_InsertParent(GEN_Tree *t, GEN_Node *n);

const char *GEN_CellBordersFormat(TokenType t);

const char *GEN_CheckIfRuleName(PrsrNdType type);

GEN_TokenType GEN_Ttype(GEN_Node *n);

void GEN_PrintNode(FILE *f, GEN_Node *n);

void GEN_ConnectNode(FILE *f, GEN_Node *n);

void GEN_DebugTree(GEN_Tree *t);

void GEN_Parent(GEN_Tree *t);

GEN_Node *GEN_GetChild(GEN_Node *n, uint64_t idx);

void GEN_AppendTree(GEN_Tree *first, GEN_Tree *second);

GEN_Node *GEN_CreateNodeByType(GEN_Tree *t, GEN_TokenType type);

GEN_Node *GEN_CreateNode(GEN_Tree *t, GEN_Token *token);


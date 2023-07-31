#include <MchlkrpchLogger/logger.h>

#include <src/Tokenizer_GEN.h>
GEN_Context GEN_TryToken(GEN_Tree *t, GEN_Token *sequence, GEN_TokenType expected_type, GEN_Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "TryToken start t(%s|idx:%lu)\n", sequence[ctx.cur_token_idx].txt, ctx.cur_token_idx);
	GEN_Tree try_token_tree = {0};
	GEN_AddChild(&try_token_tree, GEN_CreateNode(&try_token_tree, sequence + n_tokens - 1));
	if (sequence[ctx.cur_token_idx].type == expected_type) {
		GEN_AddChild(&try_token_tree, GEN_CreateNode(&try_token_tree, sequence + ctx.cur_token_idx));
		++ctx.n_parsed;
		++ctx.cur_token_idx;
	} else { return ctx; }

	GEN_Parent(&try_token_tree);
	GEN_AppendTree(t, &try_token_tree);
	__msg(D_PARSER_WORK, M, "END of TryToken\n");
	return ctx;
}

GEN_Tree *ParseSequence(GEN_Tree *t, GEN_Token *s, GEN_Context ctx, int64_t n_tokens) {
	Try_translation_unit(t, s, ctx, n_tokens);
	return t;
}

GEN_Context Try_identifier_1(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "identifier\n");
	GEN_Context try_ctx = ctx;
	GEN_Tree identifier_1_tree = {0};
	GEN_AddChild(&identifier_1_tree, GEN_CreateNodeByType(&identifier_1_tree, identifier_1));
	GEN_Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = GEN_TryToken(&identifier_1_tree, sequence, NAME_, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_identifier_1\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	GEN_AppendTree(t, &identifier_1_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_identifier_1\n");
	GEN_InsertParent(t, GEN_CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

GEN_Context Try_identifier_2(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "identifier\n");
	GEN_Context try_ctx = ctx;
	GEN_Tree identifier_2_tree = {0};
	GEN_AddChild(&identifier_2_tree, GEN_CreateNodeByType(&identifier_2_tree, identifier_2));
	GEN_Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = GEN_TryToken(&identifier_2_tree, sequence, NUMBER_, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_identifier_2\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	GEN_AppendTree(t, &identifier_2_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_identifier_2\n");
	GEN_InsertParent(t, GEN_CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

GEN_Context Try_identifier(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	GEN_Context try_ctx = ctx;
	GEN_Tree identifier_tree = {0};

	GEN_AddChild(&identifier_tree, GEN_CreateNodeByType(&identifier_tree, identifier));
	GEN_Context new_ctx = {0};

	new_ctx = Try_identifier_1(&identifier_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		new_ctx = Try_identifier_2(&identifier_tree, sequence, try_ctx, n_tokens);
		if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
			__tab_decr();
			return ctx;
		}
	}

	GEN_AppendTree(t, &identifier_tree);
	return new_ctx;
}

GEN_Context Try_additive_expression_1(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "additive_expression\n");
	GEN_Context try_ctx = ctx;
	GEN_Tree additive_expression_1_tree = {0};
	GEN_AddChild(&additive_expression_1_tree, GEN_CreateNodeByType(&additive_expression_1_tree, additive_expression_1));
	GEN_Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_identifier(&additive_expression_1_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_1\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = GEN_TryToken(&additive_expression_1_tree, sequence, PLUS, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_1\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_additive_expression(&additive_expression_1_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_1\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	GEN_AppendTree(t, &additive_expression_1_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_additive_expression_1\n");
	GEN_InsertParent(t, GEN_CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

GEN_Context Try_additive_expression_2(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "additive_expression\n");
	GEN_Context try_ctx = ctx;
	GEN_Tree additive_expression_2_tree = {0};
	GEN_AddChild(&additive_expression_2_tree, GEN_CreateNodeByType(&additive_expression_2_tree, additive_expression_2));
	GEN_Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_identifier(&additive_expression_2_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_2\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	GEN_AppendTree(t, &additive_expression_2_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_additive_expression_2\n");
	GEN_InsertParent(t, GEN_CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

GEN_Context Try_additive_expression(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	GEN_Context try_ctx = ctx;
	GEN_Tree additive_expression_tree = {0};

	GEN_AddChild(&additive_expression_tree, GEN_CreateNodeByType(&additive_expression_tree, additive_expression));
	GEN_Context new_ctx = {0};

	new_ctx = Try_additive_expression_1(&additive_expression_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		new_ctx = Try_additive_expression_2(&additive_expression_tree, sequence, try_ctx, n_tokens);
		if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
			__tab_decr();
			return ctx;
		}
	}

	GEN_AppendTree(t, &additive_expression_tree);
	return new_ctx;
}

GEN_Context Try_translation_unit_1(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "translation_unit\n");
	GEN_Context try_ctx = ctx;
	GEN_Tree translation_unit_1_tree = {0};
	GEN_AddChild(&translation_unit_1_tree, GEN_CreateNodeByType(&translation_unit_1_tree, translation_unit_1));
	GEN_Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_additive_expression(&translation_unit_1_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_1\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = GEN_TryToken(&translation_unit_1_tree, sequence, SEMICOLON, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_1\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_translation_unit(&translation_unit_1_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_1\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	GEN_AppendTree(t, &translation_unit_1_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_translation_unit_1\n");
	GEN_InsertParent(t, GEN_CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

GEN_Context Try_translation_unit_2(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "translation_unit\n");
	GEN_Context try_ctx = ctx;
	GEN_Tree translation_unit_2_tree = {0};
	GEN_AddChild(&translation_unit_2_tree, GEN_CreateNodeByType(&translation_unit_2_tree, translation_unit_2));
	GEN_Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_additive_expression(&translation_unit_2_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_2\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = GEN_TryToken(&translation_unit_2_tree, sequence, SEMICOLON, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_2\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	GEN_AppendTree(t, &translation_unit_2_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_translation_unit_2\n");
	GEN_InsertParent(t, GEN_CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

GEN_Context Try_translation_unit(GEN_Tree *t, GEN_Token *sequence, GEN_Context ctx, uint64_t n_tokens)
{
	GEN_Context try_ctx = ctx;
	GEN_Tree translation_unit_tree = {0};

	GEN_AddChild(&translation_unit_tree, GEN_CreateNodeByType(&translation_unit_tree, translation_unit));
	GEN_Context new_ctx = {0};

	new_ctx = Try_translation_unit_1(&translation_unit_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
		new_ctx = Try_translation_unit_2(&translation_unit_tree, sequence, try_ctx, n_tokens);
		if (memcmp(&new_ctx, &try_ctx, sizeof(GEN_Context)) == 0) {
			__tab_decr();
			return ctx;
		}
	}

	GEN_AppendTree(t, &translation_unit_tree);
	return new_ctx;
}


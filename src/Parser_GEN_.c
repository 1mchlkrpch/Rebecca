#include <src/Parser_GEN_.h>
#include<MchlkrpchLogger/logger.h>

Context TryToken(Tree *t, Token *sequence, TokenType expected_type, Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "TryToken start t(%s|idx:%lu)\n", sequence[ctx.cur_token_idx].txt, ctx.cur_token_idx);
	Tree try_token_tree = {0};
	AddChild(&try_token_tree, CreateNode(&try_token_tree, sequence + n_tokens - 1));
	if (sequence[ctx.cur_token_idx].type == expected_type) {
		AddChild(&try_token_tree, CreateNode(&try_token_tree, sequence + ctx.cur_token_idx));
		++ctx.n_parsed;
		++ctx.cur_token_idx;
	} else { return ctx; }

	Parent(&try_token_tree);
	AppendTree(t, &try_token_tree);
	__msg(D_PARSER_WORK, M, "END of TryToken\n");
	return ctx;
}

Context Try_additive_expression_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "additive_expression\n");
	Context try_ctx = ctx;
	Tree additive_expression_1_tree = {0};
	AddChild(&additive_expression_1_tree, CreateNode(&additive_expression_1_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = TryToken(&additive_expression_1_tree, sequence, TOKEN_NAME, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_1\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = TryToken(&additive_expression_1_tree, sequence, TOKEN_PLUS, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_1\n");
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
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_1\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	AppendTree(t, &additive_expression_1_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_additive_expression_1\n");
	InsertParent(t, CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

Context Try_additive_expression_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "additive_expression\n");
	Context try_ctx = ctx;
	Tree additive_expression_2_tree = {0};
	AddChild(&additive_expression_2_tree, CreateNode(&additive_expression_2_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = TryToken(&additive_expression_2_tree, sequence, TOKEN_NAME, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_2\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = TryToken(&additive_expression_2_tree, sequence, TOKEN_MINUS, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_2\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_additive_expression(&additive_expression_2_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_2\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	AppendTree(t, &additive_expression_2_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_additive_expression_2\n");
	InsertParent(t, CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

Context Try_additive_expression_3(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "additive_expression\n");
	Context try_ctx = ctx;
	Tree additive_expression_3_tree = {0};
	AddChild(&additive_expression_3_tree, CreateNode(&additive_expression_3_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = TryToken(&additive_expression_3_tree, sequence, TOKEN_NAME, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_additive_expression_3\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	AppendTree(t, &additive_expression_3_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_additive_expression_3\n");
	InsertParent(t, CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

Context Try_additive_expression(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree additive_expression_tree = {0};

	AddChild(&additive_expression_tree, CreateNode(&additive_expression_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	new_ctx = Try_additive_expression_1(&additive_expression_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		new_ctx = Try_additive_expression_2(&additive_expression_tree, sequence, try_ctx, n_tokens);
		if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
			new_ctx = Try_additive_expression_3(&additive_expression_tree, sequence, try_ctx, n_tokens);
			if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
				return ctx;
			}
		}
	}

	AppendTree(t, &additive_expression_tree);
	return new_ctx;
}

Context Try_translation_unit_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "translation_unit\n");
	Context try_ctx = ctx;
	Tree translation_unit_1_tree = {0};
	AddChild(&translation_unit_1_tree, CreateNode(&translation_unit_1_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_additive_expression(&translation_unit_1_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_1\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = TryToken(&translation_unit_1_tree, sequence, TOKEN_SEMICOLON, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_1\n");
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
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_1\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	AppendTree(t, &translation_unit_1_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_translation_unit_1\n");
	InsertParent(t, CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

Context Try_translation_unit_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "translation_unit\n");
	Context try_ctx = ctx;
	Tree translation_unit_2_tree = {0};
	AddChild(&translation_unit_2_tree, CreateNode(&translation_unit_2_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = Try_additive_expression(&translation_unit_2_tree, sequence, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_2\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	new_ctx = TryToken(&translation_unit_2_tree, sequence, TOKEN_SEMICOLON, try_ctx, n_tokens);
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_translation_unit_2\n");
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	AppendTree(t, &translation_unit_2_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_translation_unit_2\n");
	InsertParent(t, CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

Context Try_translation_unit(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree translation_unit_tree = {0};

	AddChild(&translation_unit_tree, CreateNode(&translation_unit_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	new_ctx = Try_translation_unit_1(&translation_unit_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		new_ctx = Try_translation_unit_2(&translation_unit_tree, sequence, try_ctx, n_tokens);
		if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
			return ctx;
		}
	}

	AppendTree(t, &translation_unit_tree);
	return new_ctx;
}


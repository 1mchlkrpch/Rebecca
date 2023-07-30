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

Context Try_rule_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "rule\n");
	Context try_ctx = ctx;
	Tree rule_1_tree = {0};
	AddChild(&rule_1_tree, CreateNode(&rule_1_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_rule_1\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	AppendTree(t, &rule_1_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_rule_1\n");
	InsertParent(t, CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

Context Try_rule_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	__msg(D_PARSER_WORK, M, "rule\n");
	Context try_ctx = ctx;
	Tree rule_2_tree = {0};
	AddChild(&rule_2_tree, CreateNode(&rule_2_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	__tab_incr();
	__msg(D_PARSER_WORK, M, "new chain in option\n");
	__tab_incr();
	__tab_decr();
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		__msg(D_PARSER_WORK, M, "NOT NEEDED OPTION: Try_rule_2\n");
		__tab_decr();
		return ctx;
	}
	try_ctx = new_ctx;
	__msg(D_PARSER_WORK, M, "END chain in option\n");
	__tab_decr();

	AppendTree(t, &rule_2_tree);
	__msg(D_PARSER_WORK, M, "EXACTLY Try_rule_2\n");
	InsertParent(t, CreateNode(t, sequence + n_tokens - 1));
	return try_ctx;
}

Context Try_rule(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree rule_tree = {0};

	AddChild(&rule_tree, CreateNode(&rule_tree, sequence + n_tokens - 1));
	Context new_ctx = {0};

	new_ctx = Try_rule_1(&rule_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		new_ctx = Try_rule_2(&rule_tree, sequence, try_ctx, n_tokens);
		if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
			__tab_decr();
			return ctx;
		}
	}

	AppendTree(t, &rule_tree);
	return new_ctx;
}


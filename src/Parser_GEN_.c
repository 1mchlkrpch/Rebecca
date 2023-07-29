#pragma once
#include <src/Parser_GEN_.h>

Context TryToken(Tree *t, Token *sequence, TokenType expected_type, Context ctx, uint64_t n_tokens)
{
	Tree try_token_tree = {0};
	if (sequence[ctx.cur_token_idx].type == expected_type) {
		Node *new_node = NodeCtor(sequence + ctx.cur_token_idx);
		AddChild(t, new_node);

		++ctx.n_parsed;
		++ctx.cur_token_idx;
	}

	Parent(&try_token_tree);
	AppendTree(t, &try_token_tree);
	return ctx;
}

Context Try_additive_expression_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree additive_expression_1_tree = {0};
	AddChild(&additive_expression_1_tree, NodeCtor(sequence + n_tokens - 1));
	Context new_ctx = {0};

	AddChild(&additive_expression_1_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = TryToken(&additive_expression_1_tree, sequence, TOKEN_NAME, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&additive_expression_1_tree);

	AddChild(&additive_expression_1_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = TryToken(&additive_expression_1_tree, sequence, TOKEN_PLUS, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&additive_expression_1_tree);

	AddChild(&additive_expression_1_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = Try_additive_expression(&additive_expression_1_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&additive_expression_1_tree);

	AppendTree(t, &additive_expression_1_tree);
	return try_ctx;
}

Context Try_additive_expression_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree additive_expression_2_tree = {0};
	AddChild(&additive_expression_2_tree, NodeCtor(sequence + n_tokens - 1));
	Context new_ctx = {0};

	AddChild(&additive_expression_2_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = TryToken(&additive_expression_2_tree, sequence, TOKEN_NAME, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&additive_expression_2_tree);

	AddChild(&additive_expression_2_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = TryToken(&additive_expression_2_tree, sequence, TOKEN_MINUS, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&additive_expression_2_tree);

	AddChild(&additive_expression_2_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = Try_additive_expression(&additive_expression_2_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&additive_expression_2_tree);

	AppendTree(t, &additive_expression_2_tree);
	return try_ctx;
}

Context Try_additive_expression_3(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree additive_expression_3_tree = {0};
	AddChild(&additive_expression_3_tree, NodeCtor(sequence + n_tokens - 1));
	Context new_ctx = {0};

	AddChild(&additive_expression_3_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = TryToken(&additive_expression_3_tree, sequence, TOKEN_NAME, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&additive_expression_3_tree);

	AppendTree(t, &additive_expression_3_tree);
	return try_ctx;
}

Context Try_additive_expression(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree additive_expression_tree = {0};

	AddChild(&additive_expression_tree, NodeCtor(sequence + n_tokens - 1));
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
	Context try_ctx = ctx;
	Tree translation_unit_1_tree = {0};
	AddChild(&translation_unit_1_tree, NodeCtor(sequence + n_tokens - 1));
	Context new_ctx = {0};

	AddChild(&translation_unit_1_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = Try_translation_unit(&translation_unit_1_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&translation_unit_1_tree);

	AddChild(&translation_unit_1_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = TryToken(&translation_unit_1_tree, sequence, TOKEN_SEMICOLON, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&translation_unit_1_tree);

	AppendTree(t, &translation_unit_1_tree);
	return try_ctx;
}

Context Try_translation_unit_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree translation_unit_2_tree = {0};
	AddChild(&translation_unit_2_tree, NodeCtor(sequence + n_tokens - 1));
	Context new_ctx = {0};

	AddChild(&translation_unit_2_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = Try_translation_unit(&translation_unit_2_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&translation_unit_2_tree);

	AddChild(&translation_unit_2_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = Try_translation_unit(&translation_unit_2_tree, sequence, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&translation_unit_2_tree);

	AddChild(&translation_unit_2_tree, NodeCtor(sequence + new_ctx.cur_token_idx));
	new_ctx = TryToken(&translation_unit_2_tree, sequence, TOKEN_SEMICOLON, try_ctx, n_tokens);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) != 0) {
		return ctx;
	}
	try_ctx = new_ctx;
	Parent(&translation_unit_2_tree);

	AppendTree(t, &translation_unit_2_tree);
	return try_ctx;
}

Context Try_translation_unit(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree translation_unit_tree = {0};

	AddChild(&translation_unit_tree, NodeCtor(sequence + n_tokens - 1));
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


#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <include/RebeccaCompiler.h>

Context try_additive_expression(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree additive_expression_tree = {0};

	AddChild(&additive_expression_tree, sequence[n_tokens - 1]);
	Context new_ctx = {0};

	new_ctx = Try_additive_expression_1(&additive_expression_tree, sequence, try_ctx);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		new_ctx = Try_additive_expression_2(&additive_expression_tree, sequence, try_ctx);
		if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
			new_ctx = Try_additive_expression_3(&additive_expression_tree, sequence, try_ctx);
			if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
				return ctx;
			}
		}
	}

	AppendTree(t, additive_expression_tree);
	return new_ctx;
}

Context try_translation_unit(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens)
{
	Context try_ctx = ctx;
	Tree translation_unit_tree = {0};

	AddChild(&translation_unit_tree, sequence[n_tokens - 1]);
	Context new_ctx = {0};

	new_ctx = Try_translation_unit_1(&translation_unit_tree, sequence, try_ctx);
	if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
		new_ctx = Try_translation_unit_2(&translation_unit_tree, sequence, try_ctx);
		if (memcmp(&new_ctx, &try_ctx, sizeof(Context)) == 0) {
			return ctx;
		}
	}

	AppendTree(t, translation_unit_tree);
	return new_ctx;
}


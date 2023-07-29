#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <include/RebeccaCompiler.h>

Context TryToken(Tree *t, Token *sequence, TokenType expected_type, Context ctx, uint64_t n_tokens);
Context Try_multiplicative_expression_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_multiplicative_expression_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_multiplicative_expression_3(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_multiplicative_expression(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_additive_expression_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_additive_expression_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_additive_expression_3(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_additive_expression(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_comp_expression_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_comp_expression(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_inner_expr_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_inner_expr_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_inner_expr(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_if_statement_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_if_statement(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_translation_unit_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_translation_unit_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_translation_unit(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);

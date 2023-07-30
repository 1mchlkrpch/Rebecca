#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <include/RebeccaGenerator.h>

Context TryToken(Tree *t, Token *sequence, TokenType expected_type, Context ctx, uint64_t n_tokens);
Context Try_rule_1(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_rule_2(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);
Context Try_rule(Tree *t, Token *sequence, Context ctx, uint64_t n_tokens);

#include <Tokenizer_GEN.h>

static inline __attribute__((always_inline))
bool GEN_IsSplit(const char c)
{ return strchr(GEN_splitters, c) != NULL; }

static inline __attribute__((always_inline))
bool GEN_IsWhiteSpace(const char c)
{ return strchr(GEN_white_space, c) != NULL; }

int64_t GEN_IdentifyToken(char *cur_word)
{
	assert(cur_word != NULL && "nullptr param");
	for (size_t idx = 0; idx < sizeof(GEN_stable_words) / sizeof(GEN_StableWord); ++idx) {
		if (strcmp(GEN_stable_words[idx].txt, cur_word) == 0) {
			return idx;
		}
	}

	return kUndefinedStableWordIdx;
}

bool GEN_CanBeAppended(char *cur_word, char *cursor, uint64_t cur_token_len)
{
	char *copy = (char *)calloc(kTokenMaxLen, sizeof(char));
	assert(copy != NULL && "Null calloc allocation");
	strcpy(copy, cur_word);
	copy[cur_token_len] = *cursor;
	if (GEN_IdentifyToken(copy) == kUndefinedStableWordIdx) {
		return false;
	} else {
		return true;
	}
}

GEN_Token GEN_ConstructToken(uint64_t idx)
{
	GEN_Token token = {0};
	token.txt = (char *)calloc(kTokenMaxLen, sizeof(char));
	assert(token.txt != NULL && "Null calloc allocation");
	token.type = GEN_stable_words[idx].type;
	strcpy(token.txt, GEN_stable_words[idx].txt);
	return token;
}

GEN_Token GEN_FillToken(const char *txt)
{
	GEN_Token token = {0};
	token.txt = (char *)calloc(kTokenMaxLen, sizeof(char));
	assert(token.txt != NULL && "Null calloc allocation");
	token.type = 0;
	strcpy(token.txt, txt);
	return token;
}

void GEN_PushToken(GEN_Token *token, GEN_Token *sequence, uint64_t *sequence_size)
{
	sequence[*sequence_size] = *token;
	++(*sequence_size);
}

void GEN_TryPushToken(
	uint64_t *cur_token_len, char *cur_word, GEN_Token *sequence, uint64_t *sequence_size)
{
	if (*cur_token_len > 0) {
		uint64_t idx = GEN_IdentifyToken(cur_word);
		GEN_Token token = {0};
		if (idx != kUndefinedStableWordIdx) {
			token = GEN_ConstructToken(idx);
		} else {
			bool is_regex = false;
			regex_t regex;
			for (size_t idx = 0; idx < sizeof(GEN_stable_regex_words) / sizeof(GEN_StableWord); ++idx) {
				int reti = regcomp(&regex, GEN_stable_regex_words[idx].txt, REG_EXTENDED | REG_NOSUB);
				if (reti == 0) {
					int res = regexec(&regex, cur_word, 0, NULL, 0);
					if (res == 0) {
						printf("YES REGEX!(%s)\n", cur_word);
						token.txt = (char *)calloc(kTokenMaxLen, sizeof(char));
						assert(token.txt != NULL && "Null calloc allocation");
						token.type = GEN_stable_regex_words[idx].type;
						strcpy(token.txt, cur_word);
						is_regex = true;
					}
				}
			}
			if (!is_regex) {
				token = GEN_FillToken(cur_word);
			}
		}
		GEN_PushToken(&token, sequence, sequence_size);
		*cur_token_len = 0;
		memset(cur_word, '\0', kTokenMaxLen);
	}
}

void GEN_PushEof(uint64_t *cur_token_len, char *cur_word, GEN_Token *sequence, uint64_t *n_tokens)
{
	*cur_token_len = 3;
	memset(cur_word, '\0', kTokenMaxLen);
	strcpy(cur_word, "EOF");
	GEN_TryPushToken(cur_token_len, cur_word, sequence, n_tokens);
}

const char *GEN_TranslateTokenType(GEN_TokenType type)
{
	switch (type) {
		case PLUS: { return "PLUS"; }
		case SEMICOLON: { return "SEMICOLON"; }
		case EOF_T: { return "EOF_T"; }
		case NUMBER_: { return "NUMBER_"; }
		case NAME_: { return "NAME_"; }
		case identifier: { return "identifier"; }
		case identifier_1: { return "identifier_1"; }
		case identifier_2: { return "identifier_2"; }
		case additive_expression: { return "additive_expression"; }
		case additive_expression_1: { return "additive_expression_1"; }
		case additive_expression_2: { return "additive_expression_2"; }
		case translation_unit: { return "translation_unit"; }
		case translation_unit_1: { return "translation_unit_1"; }
		case translation_unit_2: { return "translation_unit_2"; }
		default: {
			return "default_token";
		}
	}
	return "default_token";
}

GEN_Token *GEN_Tokenizer(char *source_text, uint64_t *n_tokens)
{
	assert(source_text != NULL && "nullptr param");
	assert(n_tokens    != NULL && "nullptr param");

	char *cursor = source_text;
	GEN_Token *sequence = (GEN_Token *)calloc(kInitSequenceSize, sizeof(GEN_Token));
	assert(sequence != NULL && "Null calloc allocation");
	uint64_t cur_token_len = 0;
	char cur_word[kTokenMaxLen] = "";
	while (*cursor != EOF) {
		if (GEN_IsWhiteSpace(*cursor)) {
			while (GEN_IsWhiteSpace(*cursor) && *cursor != EOF) {
				++cursor;
			}
			GEN_TryPushToken(&cur_token_len, cur_word, sequence, n_tokens);
			continue;
		}
		if (GEN_IsSplit(*cursor)) {
			GEN_TryPushToken(&cur_token_len, cur_word, sequence, n_tokens);
			while (GEN_IsSplit(*cursor)  && *cursor != EOF) {
				cur_word[cur_token_len] = *cursor;
				++cur_token_len;
				++cursor;
				if (GEN_CanBeAppended(cur_word, cursor, cur_token_len)) {
					continue;
				} else {
					break;
				}
			}
			GEN_TryPushToken(&cur_token_len, cur_word, sequence, n_tokens);
			continue;
		}
		cur_word[cur_token_len] = *cursor;
		++cur_token_len;
		++cursor;
	}
	GEN_TryPushToken(&cur_token_len, cur_word, sequence, n_tokens);
	GEN_PushEof(&cur_token_len, cur_word, sequence, n_tokens);
	free(source_text);

	return sequence;
}


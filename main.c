#include <stdio.h>

#include <include/RebeccaGenerator.h>
#include <MchlkrpchLogger/logger.h>

int main() {
	SetTabSize(2);
	__msg(D_TOKENIZER, M, "Start of tokenizer work\n");

	// TODO: Add analysis of tokens.
	uint64_t n_tokens = 0;
	Token *sequence = Tokenizer("../include/new_format.rbc", &n_tokens);
	
	__spt(D_TOKENIZER);
	__msg(D_TOKENIZER_OUTPUT, M, "Output of tokenizer:\n");

	for (uint64_t cur_token = 0; cur_token < n_tokens; ++cur_token) {
		__msg(D_TOKENIZER_OUTPUT, M,
			"t(%zu)|%s -- %s\n",
			cur_token,
			sequence[cur_token].txt,
			TranslateTokenType(sequence[cur_token].type));
	}
	
	__spt(D_TOKENIZER_OUTPUT);
	GenerateFiles(sequence, n_tokens);
	// End of parser-generator's work work.
	__msg(D_PARSER_GENERATING, M,
		"End of generating parser's file\n");

	return 0;
}
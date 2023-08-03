#include <stdio.h>

#include <include/RebeccaGenerator.h>
#include <MchlkrpchLogger/logger.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Pleace choose YACC-similar file!\n");
		return 0;
	}

	SetTabSize(2);
	msg(D_TOKENIZER, M,
			"Start of tokenizer work.\n");

	uint64_t n_tokens = 0;  
	// for example: "../include/function.rbc".
	const char *program_to_read = argv[1];

	char *source_text = GetSourceText(program_to_read);
	assert(source_text != NULL && "Null source text or wrong path to file!");

	msg(D_TOKENIZER, M,
			"Readen program(%s):\n%s\n",
			program_to_read,
			source_text);

	Token *sequence = Tokenizer(source_text, &n_tokens);
	free(source_text);

	spt(D_TOKENIZER);

	msg(D_TOKENIZER_OUTPUT, M,
		"Output of tokenizer:\n");

	for (uint64_t cur_token = 0; cur_token < n_tokens; ++cur_token) {
		msg(D_TOKENIZER_OUTPUT, M,
			"t(%zu)|%s -- %s\n",
			cur_token,
			sequence[cur_token].txt,
			TranslateTokenType(sequence[cur_token].type));
	}
	
	spt(D_TOKENIZER_OUTPUT);
	GenerateFiles(sequence, n_tokens);
	// End of parser-generator's work work.
	msg(D_PARSER_GENERATING, M,
		"End of generating parser's file\n");

	return 0;
}
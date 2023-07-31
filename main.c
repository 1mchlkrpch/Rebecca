#include <stdio.h>

#include <include/RebeccaGenerator.h>
#include <src/Tokenizer_GEN.h>
#include <MchlkrpchLogger/logger.h>

int main() {
	// SetLogfile(fopen("../logfile.txt", "w"));
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

	

	// // Part of usage of generated functions.
	uint64_t n_tokens2 = 0;
	char *source = GetSourceText("../examples/expr.rbc");
	GEN_Token *sequence2 = GEN_Tokenizer(source, &n_tokens2);
	for (uint64_t cur_token = 0; cur_token < n_tokens2; ++cur_token) {
		printf("t(%zu)|%s -- %s\n",
			cur_token,
			sequence2[cur_token].txt,
			GEN_TranslateTokenType(sequence2[cur_token].type));
	}



	GEN_Tree t = {0};
	GEN_Context ctx = {0};
	GEN_AddChild(&t, GEN_CreateNode(&t, sequence2 + n_tokens2 - 1));
	ParseSequence(&t, sequence2, ctx, n_tokens2);
	GEN_CompressTree(&t, t.root);

	GEN_DebugTree(&t);

	return 0;
}
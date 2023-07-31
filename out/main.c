#include <stdio.h>

#include <Tokenizer_GEN.h>
#include <../MchlkrpchLogger/logger.h>

char *GetSourceText(const char *name)
{
  assert(name != NULL && "nullptr param");
  
  __tab_incr();

  FILE *f = fopen(name, "r");
  assert(f != NULL && "Open file error");

  fseek(f, 0, SEEK_END);
  /* Contains number of symbols wihtout EOF-symbol.
  For example with file "abaEOF" n_symbols = 3.*/
  size_t n_symbols = ftell(f);
  __msg(D_TOKENIZER, M, "mush be allocated:%zu\n", n_symbols);
  fseek(f, 0, SEEK_SET);

  char *source_text = (char *)calloc(n_symbols + 1, sizeof(char));
  fread(source_text, sizeof(char), n_symbols, f);

  /* Set last character as 'EOF' symbol
  to terminate tokenizer's work*/
  source_text[n_symbols] = EOF;
  fclose(f);
  __tab_decr();

  return source_text;
}

int main() {
	uint64_t n_tokens2 = 0;
	char *source = GetSourceText("../../examples/expr.rbc");
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
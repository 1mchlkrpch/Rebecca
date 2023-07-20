#include <include/RebeccaCompiler.h>

Parser *BuildAst(Token *sequence, uint64_t n_tokens)
{
	assert(sequence != NULL && "nullptr param");

	Parser *parser = (Parser *)calloc(1, sizeof(Parser));
	assert(parser != NULL && "Null calloc allocation");
	(void)n_tokens;

	// Add tokens
	// Add png debugger
	// syntax analysis

	return parser;
}

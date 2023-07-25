#define D_ASSERTS           0x000001
#define D_TOKENIZER         0x000010
#define D_TOKENIZER_OUTPUT  0x000100
#define D_PARSER_GENERATING 0x001000
// Off all debug prints.
#define NO_DEBUG            0x100000

#define chosen_group (D_TOKENIZER_OUTPUT | D_PARSER_GENERATING)
#define D_ASSERTS           0x0000001
#define D_TOKENIZER         0x0000010
#define D_TOKENIZER_OUTPUT  0x0000100
#define D_PARSER_GENERATING 0x0001000
#define D_NAMETABLE         0x0010000
#define D_FILE_PRINT        0x0100000
// Off all debug prints.
#define NO_DEBUG            0x1000000

#define chosen_group (D_FILE_PRINT | D_PARSER_GENERATING | D_NAMETABLE)
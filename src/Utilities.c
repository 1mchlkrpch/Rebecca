#include <include/Utilities.h>
#include <include/RebeccaGenerator.h>
#include <MchlkrpchLogger/logger.h>

#include <stdio.h>

/**
 * @brief returns source text
 * which is come from FILE with name 'name'.
 * 
 * @param     name Name of file to read.
 * @returns   source_text char* text of file.
 */
char *GetSourceText(const char *name)
{
  assert(name != NULL && "nullptr param");
  
  tab_incr();

  FILE *f = fopen(name, "r");
  assert(f != NULL && "Open file error");

  fseek(f, 0, SEEK_END);
  /* Contains number of symbols wihtout EOF-symbol.
  For example with file "abaEOF" n_symbols = 3.*/
  size_t n_symbols = ftell(f);
  msg(D_TOKENIZER, M,
    "mush be allocated:%zu\n", n_symbols);
  fseek(f, 0, SEEK_SET);

  char *source_text = (char *)calloc(n_symbols + 1, sizeof(char));
  fread(source_text, sizeof(char), n_symbols, f);

  /* Set last character as 'EOF' symbol
  to terminate tokenizer's work*/
  source_text[n_symbols] = EOF;
  
  fclose(f);

  tab_decr();
  // printf("source:(%c)\n", source_text[0]);

  return source_text;
}

/**
 * @brief constructor of 'Array'-structure.
 * Allocates one array with allocated array of children
 * to one element.
 * 
 * @param el_sz   Size of element to contain.
 * @returns       Allocated 'Array' structure.
 */
Array *ArrayCtor(uint64_t el_sz)
{
	assert(el_sz > 0 && "Element size should be positive number");
	
	Array *a = (Array *)calloc(1, sizeof(Array));
	assert(a != NULL && "Null calloc allocation");

	a->size         = 0;
	a->capacity     = 1;
	a->element_size = el_sz;

	a->data = calloc(1, el_sz * sizeof(char));
	assert(a->data != NULL && "Null calloc allocation");

	return a;
}


/**
 * @brief fills NULL-buffer with data from 'src'.
 * Firstly allocates memory then copies 'src' string to 'dest'.
 * 
 * @param dest   Destination buffer to fill.
 * @param src    Source buffer.
 */
void FillEmptyStr(char **dest, const char *src)
{
	assert(src  != NULL && "Null param");

	(*dest) = (char *)calloc(kTokenMaxLen, sizeof(char));
	assert(dest != NULL && "Null calloc allocation");

	strcpy(*dest, src);
}

/**
 * @brief Changes array's capacity to 'new_capacity'.
 * 
 * @param a              Array to change.
 * @param new_capacity   New capacity.
 */
void ArrayChangeCapacity(Array *a, uint64_t new_capacity)
{
	assert(a != NULL && "Null param");

	a->data = realloc(a->data, a->element_size * new_capacity * sizeof(char));
	assert(a->data != NULL && "Null reallocation");

	a->capacity = new_capacity;
}

/**
 * @brief Adds to array new element.
 * If it's necessary increases Array::data capacity.
 * 
 * @param a             Array to append.
 * @param new_element   pointer to data to append with.
 */
void ArrayAdd(Array *a, void *new_element)
{
	if (a->size == a->capacity) {
		ArrayChangeCapacity(a, a->capacity << 1);
	}
	
	void *ptr = (char *)a->data + a->size * a->element_size;
	memcpy(ptr, &new_element, a->element_size);
	++a->size;
}

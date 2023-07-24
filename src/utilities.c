#include <include/utilities.h>
#include <include/RebeccaCompiler.h>

#include <stdio.h>

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

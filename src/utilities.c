#include <include/utilities.h>
#include <include/RebeccaCompiler.h>

#include <stdio.h>

Array *ArrayCtor(uint64_t el_sz)
{
	Array *a = (Array *)calloc(1, sizeof(Array));

	a->size     = 0;
	a->capacity = 1;
	a->element_size = el_sz;

	a->data = calloc(1, el_sz * sizeof(char));
	// printf("size of el in arr:%zu\n", el_sz * sizeof(char));

	return a;
}

void ArrayChangeCapacity(Array *a, uint64_t new_capacity)
{
	assert(a != NULL && "Null param");

	a->data = realloc(a->data, a->element_size * new_capacity * sizeof(char));
	// printf("change to (in bytes):%ld", a->element_size * new_capacity * sizeof(char));
	assert(a->data != NULL && "Null reallocation");
	a->capacity = new_capacity;
}

void ArrayAdd(Array *a, void *new_element)
{
	if (a->size == a->capacity) {
		ArrayChangeCapacity(a, a->capacity << 1);
		// printf("Increase of capacity:%ld\n", a->capacity);
	}

	// printf("elsize in array ad:%zu\n", a->size * a->element_size);
	// printf("::%s\n", ((Node*)new_element)->token.txt);
	// printf("::sz:%ld\n", a->element_size);
	// printf("array a:sz:%ld\n", a->size);

	void *ptr = (char *)a->data + a->size * a->element_size;
	memcpy(&ptr, &new_element, a->element_size);
	// printf("after memcpy:%s|from:%s\n", ((Node*)ptr)->token.txt, ((Node*)new_element)->token.txt);
	++a->size;
}

/**
 * @file utilities.h
 * Contains some auxilary functions and structures for Rebecca compiler.
 */
#pragma once

#include <assert.h>

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief Auxilary data structure to
 * provide n-ary trees.
 * 
 * data is array of pointers to children.
 * Each node has 0-n children but user should
 * do conversion of types every time he use data-array.
 */
typedef struct
{
	// Number of descendants of each node.
	uint64_t size;
	// Capacity of children array.
	uint64_t capacity;
	// Size of containable type.
	uint64_t element_size;
	// Children array.
	void *data;
} Array;

Array *ArrayCtor(uint64_t el_sz);

void ArrayChangeCapacity(Array *a, uint64_t new_capacity);

void ArrayAdd(Array *a, void *new_element);

void FillEmptyStr(char **dest, const char *src);

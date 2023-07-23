/**
 * @file utilities.h
 * Contains some auxilary functions and structures for Rebecca compiler.
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct
{
	uint64_t size;
	uint64_t capacity;

	uint64_t element_size;

	void *data;
} Array;

Array *ArrayCtor(uint64_t el_sz);

void ArrayChangeCapacity(Array *a, uint64_t new_capacity);

void ArrayAdd(Array *a, void *new_element);

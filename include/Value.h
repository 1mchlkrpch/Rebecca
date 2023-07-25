#pragma once

typedef enum
{
  // Local scope.
  SCOPE_LOCAL,
  // Upper scope.
  SCOPE_UPPER,
  // Global scope.
  SCOPE_GLOBAL
} Scope;

// Identifies which specific type a heap-allocated object is.
typedef enum {
  	// Complicated types.
  OBJ_CLASS,
  OBJ_STRUCT,
  OBJ_FUNCITON,
  OBJ_INSTANCE,
  	// Obvious c-types
  OBJ_INT,
  OBJ_STRING,
  OBJ_SYM,
  	/* Another for example type of entities
  	while generating ast of parser generator.*/
  OBJ_ANOTHER
} ObjType;

/**
 * @addtogroup Variables
 * @{
 */
typedef struct
{
  size_t size_of_object;
  ObjType type;

  void *obj;
} Value;
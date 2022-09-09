#pragma once

#include <stdlib.h>

typedef enum
{
	INT,
	PTR,
	ARRAY,
	CHAR,
	STRUCT,
} TypeKind;

typedef struct Type Type;
typedef struct StructField StructField;
struct StructField
{
	StructField *next;
	char *name;
	int len;
	Type *type;
	int offset;
};

typedef struct StructType StructType;
struct StructType
{
	StructType *next;
	char *name;
	int len;
	StructField *fields;
	int size;
};

struct Type
{
	TypeKind ty;
	Type *ptr_to;
	size_t array_size;
	StructType *struct_type;
};

Type *new_type(TypeKind ty, Type *ptr_to);
int sizeof_type(Type *type);
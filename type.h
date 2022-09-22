#pragma once

typedef enum
{
	INT,
	PTR,
	ARRAY,
	CHAR,
	STRUCT,
	VOID,
	FUNC,
	BOOL,
	VA_LIST_TAG
} TypeKind;

typedef struct Type Type;
typedef struct StructField StructField;
typedef struct StructType StructType;

struct StructField
{
	StructField *next;
	String *name;
	Type *type;
	int offset;
};

struct StructType
{
	StructType *next;
	String *name;
	StructField *fields;
	int alignment;
	int size;
	bool is_union;
};

struct Type
{
	TypeKind ty;
	Type *ptr_to;
	int array_size;
	StructType *struct_type;
};

Type *new_type(TypeKind ty, Type *ptr_to);
Type *new_struct_type(String *name, bool is_union);
void add_field(StructType *type, Type *ty, String *name);
int sizeof_type(Type *type);

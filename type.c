#include <stdlib.h>
#include "error.h"
#include "header.h"

Type *new_type(TypeKind ty, Type *ptr_to)
{
	Type *type = calloc(1, sizeof(Type));
	type->ty = ty;
	type->ptr_to = ptr_to;

	return type;
}

Type *new_struct_type(String *name, bool is_union)
{
	Type *ty = new_type(STRUCT, NULL);
	ty->struct_type = calloc(1, sizeof(StructType));
	ty->struct_type->name = name;
	ty->struct_type->is_union = is_union;
	return ty;
}

void add_field(StructType *type, Type *ty, String *name)
{
	StructField *field = calloc(1, sizeof(StructField));
	field->type = ty;
	field->name = name;
	if (type->fields)
		field->index = type->fields->index + 1;
	else
		field->index = 0;
	if (sizeof_type(ty) > type->alignment)
		type->alignment = sizeof_type(ty);
	field->next = type->fields;
	type->fields = field;
}

int sizeof_type(Type *type)
{
	switch (type->ty)
	{
	case INT:
		return 4;
	case PTR:
		return 8;
	case ARRAY:
		return sizeof_type(type->ptr_to) * type->array_size;
	case CHAR:
		return 1;
	case STRUCT:
		if (type->struct_type->is_union)
			return type->struct_type->alignment;
		else
			return type->struct_type->alignment * (type->struct_type->fields->index + 1);
	case BOOL:
		return 1;
	}

	error_at_here("sizeof_type: unknown type");
}
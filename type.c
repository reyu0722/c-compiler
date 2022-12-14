#ifdef __STDC__
#include <stdlib.h>
#else
void *calloc();
#endif
#include "error.h"
#include "type.h"

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

int align(int n, int al)
{
	return (n + al - 1) / al * al;
}

void add_field(StructType *type, Type *ty, String *name)
{
	StructField *field = calloc(1, sizeof(StructField));
	field->type = ty;
	field->name = name;

	int al;
	if (ty->ty == ARRAY)
		al = sizeof_type(ty->ptr_to);
	else
		al = sizeof_type(ty);

	if (al > type->alignment)
		type->alignment = al;

	if (type->is_union)
	{
		if (sizeof_type(ty) > type->size)
			type->size = sizeof_type(ty);
	}
	else
	{
		if (type->fields)
		{
			field->offset = align(type->fields->offset + sizeof_type(type->fields->type), al);
			type->size = align(field->offset + sizeof_type(field->type), type->alignment);
		}
		else
		{
			field->offset = 0;
			type->size = align(sizeof_type(field->type), type->alignment);
		}
	}

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
		return type->struct_type->size;
	case BOOL:
		return 1;
	case VA_LIST_TAG:
		return 24;
	}

	error_at_here("sizeof_type: unknown type");
}
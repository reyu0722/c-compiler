#include <stdlib.h>
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
	if (type->is_union)
	{
		field->offset = sizeof_type(ty);
		if (field->offset > type->size)
			type->size = field->offset;
	}
	else
	{
		if (type->fields)
			field->offset = type->fields->offset + sizeof_type(ty);
		else
			field->offset = sizeof_type(ty);

		type->size = field->offset;
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
	}

	error_at_here("sizeof_type: unknown type");
}
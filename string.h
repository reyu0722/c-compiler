#pragma once

#include <stdbool.h>

typedef struct String String;

struct String
{
	char *ptr;
	int len;
};

String *new_string(char *ptr, int len);
bool str_equals(String *s1, String *s2);
bool str_chr_equals(String *s1, char *s2);
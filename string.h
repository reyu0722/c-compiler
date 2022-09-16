#ifdef __STDC__
#pragma once
#include <stdbool.h>
#endif

struct String
{
	char *ptr;
	int len;
};
typedef struct String String;

String *new_string(char *ptr, int len);
_Bool str_equals(String *s1, String *s2);
_Bool str_chr_equals(String *s1, char *s2);
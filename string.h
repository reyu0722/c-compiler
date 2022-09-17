#pragma once

#ifdef __STDC__
#include <stdbool.h>
#endif

typedef struct String String;
struct String
{
	char *ptr;
	int len;
};

String *new_string(char *ptr, int len);
_Bool str_equals(String *s1, String *s2);
_Bool str_chr_equals(String *s1, char *s2);
#pragma once

#ifdef __STDC__
#include <stdbool.h>
#endif

#define bool _Bool
#define true 1
#define false 0

typedef struct String String;
struct String
{
	char *ptr;
	int len;
};

String *new_string(char *ptr, int len);
bool str_equals(String *s1, String *s2);
bool str_chr_equals(String *s1, char *s2);
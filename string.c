#ifdef __STDC__
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#else
void *calloc();
#endif
#include "string.h"

String *new_string(char *ptr, int len)
{
	String *substr = calloc(1, sizeof(String));
	substr->ptr = ptr;
	substr->len = len;
	return substr;
}

bool str_equals(String *s1, String *s2)
{
	return s1->len == s2->len && !memcmp(s1->ptr, s2->ptr, s1->len);
}

bool str_chr_equals(String *s1, char *s2)
{
	return s1->len == strlen(s2) && !memcmp(s1->ptr, s2, s1->len);
}
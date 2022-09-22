#ifdef __STDC__
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#else
typedef __builtin_va_list va_list;
extern void *stderr;
#endif
#include "header.h"
#include "tokenize.h"

void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

void _error_at(char *loc, char *fmt, va_list ap)
{
	char *line = loc;
	while (user_input < line && line[-1] != '\n')
		line--;

	char *end = loc;
	while (*end != '\n')
		end++;

	int line_num = 1;
	for (char *p = user_input; p < line; p++)
		if (*p == '\n')
			line_num++;

	int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
	fprintf(stderr, "%.*s\n", end - line, line);

	int pos = loc - line + indent;
	fprintf(stderr, "%*s", pos, "");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, &(ap[0]));
	fprintf(stderr, "\n");
	exit(1);
}

void error_at(char *loc, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	_error_at(loc, fmt, ap);
}

void error_at_token(Token *tok, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	_error_at(tok->str->ptr, fmt, ap);
}

void error_at_here(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	_error_at(token->str->ptr, fmt, ap);
}

void assert(bool flag)
{
	if (!flag)
		error_at_token(token, "assertion failed");
}

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
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

void error_at(char *loc, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

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
	fprintf(stderr, "%.*s\n", (int)(end - line), line);

	int pos = loc - line + indent;
	fprintf(stderr, "%*s", pos, "");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

void assert(bool flag)
{
  if (!flag)
    error_at(token->str, "assertion failed");
}
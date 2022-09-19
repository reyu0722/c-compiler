#ifdef __STDC__
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
void *calloc();
char *strncpy();
int snprintf();
#endif
#include "error.h"
#include "header.h"

char *once_file[100];

Token *preprocess(Token *tok)
{
	Token *before = NULL;
	Token *start = NULL;
	for (Token *t = tok; t; t = t->next)
	{
		if (t->kind == TK_PREPROCESSOR)
		{
			if (startswith(t->str->ptr, "#include"))
			{
				char *curfile = filename;
				filename = calloc(1, 100);
				char *path = calloc(1, 200);
				strncpy(filename, t->str->ptr + 10, t->str->len - 11);

				snprintf(path, 200, "%s/%s", dir_name, filename);

				char *header = read_file(path);
				Token *header_token = tokenize(header, 0); // TODO: use `false`
				header_token = preprocess(header_token);
				filename = curfile;
				if (header_token)
				{
					Token *header_token_end = header_token;
					while (header_token_end->next)
						header_token_end = header_token_end->next;

					if (before)
						before->next = header_token;
					else
						start = header_token;

					before = header_token_end;
				}
			}

			if (startswith(t->str->ptr, "#ifdef"))
			{
				while (!(startswith(t->str->ptr, "#else") || startswith(t->str->ptr, "#endif")))
					t = t->next;

				if (startswith(t->str->ptr, "#else"))
				{
					t = t->next;

					if (before)
						before->next = t;
					else
						start = t;

					while (!startswith(t->str->ptr, "#endif"))
					{
						before = t;
						t = t->next;
					}
				}
				else
				{
					assert(startswith(t->str->ptr, "#endif"));
				}
			}

			if (startswith(t->str->ptr, "#pragma once"))
			{
				int i = 0;
				while (once_file[i])
				{
					if (strcmp(once_file[i], filename) == 0)
						return NULL;
					i++;
				}
				once_file[i] = filename;
			}
		}
		else
		{
			if (before)
				before->next = t;
			else
				start = t;
			before = t;
		}
	}

	return start;
}
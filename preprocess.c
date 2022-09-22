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
#include "file.h"
#include "header.h"
#include "preprocess.h"

char *once_file[100];
Macro *macros[100];
int mi;

Macro *find_macro(String *str)
{
	for (int i = 0; i < mi; i++)
		if (str_equals(str, macros[i]->ident))
			return macros[i];
	return NULL;
}

Token *remove_newline(Token *tok)
{
	Token *before = NULL;
	for (Token *t = tok; t; t = t->next)
	{
		if (t->kind == TK_NEWLINE)
		{
			if (before)
				before->next = t->next;
			else
				tok = t->next;
		}
		else
			before = t;
	}
	return tok;
}

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

				t = t->next;
				assert(t->kind == TK_STRING);

				strncpy(filename, t->str->ptr, t->str->len);

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

			if (startswith(t->str->ptr, "#pragma"))
			{
				t = t->next;
				if (str_chr_equals(t->str, "once"))
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

			if (startswith(t->str->ptr, "#define"))
			{
				t = t->next;
				assert(t->kind == TK_IDENT);

				Macro *m = calloc(1, sizeof(Macro));
				m->ident = t->str;

				t = t->next;
				m->replace = t;

				while (t->next->kind != TK_NEWLINE)
					t = t->next;

				Token *last = t;
				t = t->next;
				last->next = NULL;

				macros[mi++] = m;
			}
		}
		else
		{
			if (t->kind == TK_IDENT)
			{
				Macro *m = find_macro(t->str);
				if (!m)
				{
					if (before)
						before->next = t;
					else
						start = t;
					before = t;
					continue;
				}

				Token *replace = m->replace;
				while (replace)
				{
					Token *newt = calloc(1, sizeof(Token));
					newt->kind = replace->kind;
					newt->val = replace->val;
					newt->str = t->str;
					if (before)
						before->next = newt;
					else
						start = t;
					before = newt;
					replace = replace->next;
				}
				continue;
			}

			if (before)
				before->next = t;
			else
				start = t;
			before = t;
		}
	}

	start = remove_newline(start);

	return start;
}
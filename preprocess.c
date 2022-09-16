#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "header.h"
#include "tokenize.h"

Token *preprocess(Token *tok)
{
	Token *before = NULL;
	Token *start;
	for (Token *t = tok; t; t = t->next)
	{
		if (t->kind == TK_PREPROCESSOR)
		{
			if (startswith(t->str->ptr, "#include"))
			{
				char filename[100] = {};
				char path[200] = {};
				strncpy(filename, t->str->ptr + 10, t->str->len - 11);

				snprintf(path, sizeof(path), "%s/%s", dir_name, filename);

				char *header = read_file(path);
				Token *header_token = tokenize(header, false);
				header_token = preprocess(header_token);
				if (!header_token)
					error("failed to tokenize %s", path);

				Token *header_token_end = header_token;
				while (header_token_end->next)
					header_token_end = header_token_end->next;

				if (before)
					before->next = header_token;
				else
					start = header_token;

				before = header_token_end;
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

					while (t->kind == TK_PREPROCESSOR && !startswith(t->str->ptr, "#endif"))
					{
						before = t;
						t = t->next;
					}

					t = t->next;
				}
				else
				{
					assert(startswith(t->str->ptr, "#endif"));
				}
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
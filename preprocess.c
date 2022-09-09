#include <stdbool.h>
#include <stdio.h>
#include <string.h>
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
			if (startswith(tok->str, "#include"))
			{
				char filename[100] = {};
				char path[200] = {};
				strncpy(filename, tok->str + 10, tok->len - 11);

				snprintf(path, sizeof(path), "%s/%s", dir_name, filename);

				char *header = read_file(path);
				Token *header_token = tokenize(header, false);
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
#ifdef __STDC__
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#else
void *calloc();
int memcmp();
char *strstr();
char *strchr();
#endif
#include "error.h"
#include "header.h"
#include "string.h"
#include "tokenize.h"

Token *token;

Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = new_string(str, len);
	cur->next = tok;
	return tok;
}

_Bool is_alnum(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_');
}

_Bool startswith(char *p, char *q)
{
	return memcmp(p, q, strlen(q)) == 0;
}

Token *tokenize(char *p, _Bool eof)
{
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p)
	{
		if (*p == '\n') {
			cur = new_token(TK_NEWLINE, cur, p++, 1);
			continue;
		}
		if (isspace(*p))
		{
			p++;
			continue;
		}

		if (startswith(p, "#"))
		{
			char *q = p;
			while (!isspace(*q))
				q++;
			cur = new_token(TK_PREPROCESSOR, cur, p, q - p);
			p = q;
			continue;
		}

		if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">="))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}
		if (startswith(p, "||") || startswith(p, "&&"))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (startswith(p, "++") || startswith(p, "--"))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (startswith(p, "+=") || startswith(p, "-=") || startswith(p, "*=") || startswith(p, "/="))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (startswith(p, "//"))
		{
			p += 2;
			while (*p != '\n')
				p++;
			continue;
		}

		if (strncmp(p, "/*", 2) == 0)
		{
			char *q = strstr(p + 2, "*/");
			if (!q)
				error_at(p, "tokenize failed: \"*/\" not found");
			p = q + 2;
			continue;
		}

		if (strncmp(p, "->", 2) == 0)
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		if (strncmp(p, "...", 3) == 0)
		{
			cur = new_token(TK_RESERVED, cur, p, 3);
			p += 3;
			continue;
		}

		if (strchr("+-*/()<>:;={},&[].!", *p))
		{
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if (*p == '"')
		{
			cur = new_token(TK_STRING, cur, ++p, 0);
			for (; *(p - 1) == '\\' || (*p != '"'); p++)
				cur->str->len++;
			p++;
			continue;
		}

		if (*p == '\'')
		{
			cur = new_token(TK_CHAR_CONST, cur, ++p, 0);
			for (; (*(p - 2) != '\\' && *(p - 1) == '\\') || *p != '\''; p++)
				cur->str->len++;
			p++;
			continue;
		}

		if (isdigit(*p))
		{
			char *newp;
			int val = strtol(p, &newp, 10);
			int len = newp - p;
			p = newp;
			cur = new_token(TK_NUM, cur, p, len);
			cur->val = val;
			continue;
		}

		if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}

		if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2]))
		{
			cur = new_token(TK_IF, cur, p, 2);
			p += 2;
			continue;
		}

		if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}

		if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5]))
		{
			cur = new_token(TK_WHILE, cur, p, 5);
			p += 5;
			continue;
		}

		if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3]))
		{
			cur = new_token(TK_FOR, cur, p, 3);
			p += 3;
			continue;
		}

		if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3]))
		{
			cur = new_token(TK_INT, cur, p, 3);
			p += 3;
			continue;
		}

		if (strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_SIZEOF, cur, p, 6);
			p += 6;
			continue;
		}

		if (strncmp(p, "char", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_CHAR, cur, p, 4);
			p += 4;
			continue;
		}

		if (strncmp(p, "enum", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_ENUM, cur, p, 4);
			p += 4;
			continue;
		}

		if (strncmp(p, "struct", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_STRUCT, cur, p, 6);
			p += 6;
			continue;
		}

		if (strncmp(p, "union", 5) == 0 && !is_alnum(p[5]))
		{
			cur = new_token(TK_UNION, cur, p, 5);
			p += 5;
			continue;
		}

		if (strncmp(p, "typedef", 7) == 0 && !is_alnum(p[7]))
		{
			cur = new_token(TK_TYPEDEF, cur, p, 7);
			p += 7;
			continue;
		}

		if (strncmp(p, "switch", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_SWITCH, cur, p, 6);
			p += 6;
			continue;
		}

		if (strncmp(p, "case", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_CASE, cur, p, 4);
			p += 4;
			continue;
		}
		if (strncmp(p, "default", 7) == 0 && !is_alnum(p[7]))
		{
			cur = new_token(TK_DEFAULT, cur, p, 7);
			p += 7;
			continue;
		}

		if (strncmp(p, "break", 5) == 0 && !is_alnum(p[5]))
		{
			cur = new_token(TK_BREAK, cur, p, 5);
			p += 5;
			continue;
		}

		if (strncmp(p, "continue", 8) == 0 && !is_alnum(p[8]))
		{
			cur = new_token(TK_CONTINUE, cur, p, 8);
			p += 8;
			continue;
		}

		if (strncmp(p, "void", 4) == 0 && !is_alnum(p[4]))
		{
			cur = new_token(TK_VOID, cur, p, 4);
			p += 4;
			continue;
		}

		if (strncmp(p, "__builtin_va_list", 17) == 0 && !is_alnum(p[17]))
		{
			cur = new_token(TK_BUILTIN_VA_LIST, cur, p, 17);
			p += 17;
			continue;
		}

		if (strncmp(p, "_Bool", 5) == 0 && !is_alnum(p[5]))
		{
			cur = new_token(TK_BOOL, cur, p, 5);
			p += 5;
			continue;
		}

		if (strncmp(p, "extern", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_EXTERN, cur, p, 6);
			p += 6;
			continue;
		}

		if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z') || (*p == '_'))
		{
			char *q;
			for (q = p; ('a' <= *q && *q <= 'z') || ('A' <= *q && *q <= 'Z') || ('0' <= *q && *q <= '9') || (*q == '_'); q++)
				q = q;

			cur = new_token(TK_IDENT, cur, p, q - p);
			p = q;
			continue;
		}

		error_at(p, "tokenize failed");
	}

	if (eof)
		new_token(TK_EOF, cur, p, 1);
	return head.next;
}
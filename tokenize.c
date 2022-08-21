#include "mycc.h"

Token *token;

Token *new_token(TokenKind kind, Token *cur, char *str)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

bool is_alnum(char c)
{
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_');
}

bool startswith(char *p, char *q)
{
  return memcmp(p, q, strlen(q)) == 0;
}

Token *tokenize(char *p)
{
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p)
  {
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">="))
    {
      cur = new_token(TK_RESERVED, cur, p);
      p += 2;
      cur->len = 2;
      continue;
    }

    if (strchr("+-*/()<>;=", *p))
    {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    if (isdigit(*p))
    {
      char *newp;
      int val = strtol(p, &newp, 10);
      int digit = (newp - p) / sizeof(char);
      p = newp;
      cur = new_token(TK_NUM, cur, p);
      cur->val = val;
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
    {
      cur = new_token(TK_RETURN, cur, p);
      p += 6;
      continue;
    }

		if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2]))
		{
			cur = new_token(TK_IF, cur, p);
			p += 2;
			continue;
		}

		if ('a' <= *p && *p <= 'z')
    {
      char *q;
      for (q = p; 'a' <= *q && *q <= 'z'; q++)
        ;

      cur = new_token(TK_IDENT, cur, p);
      cur->len = q - p;
      p = q;
      continue;
    }

    error_at(p, "tokenize failed");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}
#pragma once

typedef struct Token Token;

typedef enum
{
	TK_RESERVED,
	TK_IDENT,
	TK_NUM,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
	TK_INT,
	TK_CHAR,
	TK_ENUM,
	TK_STRUCT,
	TK_EOF,
	TK_SIZEOF,
	TK_STRING,
	TK_PREPROCESSOR
} TokenKind;

struct Token
{
	TokenKind kind;
	Token *next;
	int val;
	char *str;
	int len;
};

extern Token *token;
bool startswith(char *p, char *q);
Token *tokenize(char *p, bool eof);
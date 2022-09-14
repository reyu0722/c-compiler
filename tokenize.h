#pragma once

#include "string.h"

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
	TK_CHAR_CONST,
	TK_PREPROCESSOR,
	TK_TYPEDEF,
	TK_SWITCH,
	TK_CASE,
	TK_BREAK,
	TK_VOID,
	TK_LONG,
	TK_UNSIGNED,
	TK_SIGNED,
	TK_SHORT
} TokenKind;

struct Token
{
	TokenKind kind;
	Token *next;
	int val;
	String *str;
};

extern Token *token;
bool startswith(char *p, char *q);
Token *tokenize(char *p, bool eof);
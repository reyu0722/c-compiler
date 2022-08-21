#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token
{
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

typedef enum
{
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_ASSIGN, // =
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_RETURN, // return
  ND_IF,     // if
  ND_IFELSE, // if ... else ...
  ND_WHILE,  // while
  ND_FOR,    // for
  ND_BLOCK,  // { ... }
  ND_LVAR,   // Local Variable
  ND_NUM,    // Integer
  ND_UNNAMED
} NodeKind;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;    // only use when kind == ND_NUM
  int offset; // only use when kind == ND_LVAR
};

// main.c
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// codegen.c
extern Node *code[100];
void gen(Node *node);

// parse.c
void program();

// tokenize.c
extern Token *token;
Token *tokenize(char *p);
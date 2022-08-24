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
  ND_ADDR,   // *
  ND_DEREF,  // &
  ND_RETURN, // return
  ND_IF,     // if
  ND_IFELSE, // if ... else ...
  ND_WHILE,  // while
  ND_FOR,    // for
  ND_BLOCK,  // { ... }
  ND_LVAR,   // Local Variable
  ND_CALL,   // Function Call
  ND_NUM,    // Integer
  ND_UNNAMED
} NodeKind;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;    // ND_NUM
  int offset; // ND_LVAR
  char *name; // ND_CALL
  int len;    // ND_CALL
};

typedef struct Function Function;
struct Function
{
  char *name;
  int len;
  Node *code[100];
  int offsets[6];
};

// main.c
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// codegen.c
void gen(Node *node);

// parse.c
extern Function function;
bool at_eof();
void parse_function();

// tokenize.c
extern Token *token;
Token *tokenize(char *p);
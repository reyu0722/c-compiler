#pragma once

#include "type.h"

typedef enum
{
  ND_ADD,          // +
  ND_SUB,          // -
  ND_MUL,          // *
  ND_DIV,          // /
  ND_ASSIGN,       // =
  ND_ASSIGN_ARRAY, // = { ... }
  ND_EQ,           // ==
  ND_NE,           // !=
  ND_LT,           // <
  ND_LE,           // <=
  ND_ADDR,         // *
  ND_DEREF,        // &
  ND_RETURN,       // return
  ND_IF,           // if
  ND_IFELSE,       // if ... else ...
  ND_WHILE,        // while
  ND_FOR,          // for
  ND_BLOCK,        // { ... }
  ND_LVAR,         // Local Variable
  ND_GVAR,         // Global Variable
  ND_CALL,         // Function Call
  ND_NUM,          // Integer
  ND_STRING,       // String literals
  ND_UNNAMED
} NodeKind;

typedef enum
{
  DIR_INCLUDE
} Directive;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;    // ND_NUM
  int offset; // ND_LVAR
  Type *type; // expr
  char *name; // ND_CALL
  int len;    // ND_CALL
};

typedef struct StringLiteral StringLiteral;
struct StringLiteral
{
  StringLiteral *next;
  char *str;
  int len;
  int offset;
};

// main.c
char *dir_name;
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
char *read_file(char *path);

#pragma once

#include "string.h"
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
  ND_AND,          // &&
  ND_OR,           // ||
  ND_ADDR,         // *
  ND_DEREF,        // &
  ND_POST_INCR,    // a++
  ND_POST_DECR,    // a--
  ND_RETURN,       // return
  ND_IF,           // if
  ND_IFELSE,       // if ... else ...
  ND_SWITCH,       // switch
  ND_CASE,         // case
  ND_DEFAULT,      // default
  ND_BREAK,        // break
  ND_CONTINUE,     // continue
  ND_WHILE,        // while
  ND_FOR,          // for
  ND_BLOCK,        // { ... }
  ND_LVAR,         // Local Variable
  ND_GVAR,         // Global Variable
  ND_CALL,         // Function Call
  ND_NUM,          // Integer
  ND_STRING,       // String literals
  ND_VA_START,     // va_start()
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
  int val;      // ND_NUM
  int offset;   // ND_LVAR
  Type *type;   // expr
  String *name; // ND_CALL
};

typedef struct StringLiteral StringLiteral;
struct StringLiteral
{
  StringLiteral *next;
  String *str;
  int offset;
};

extern char *user_input;
extern char *dir_name;
extern char *filename;
extern int current_stack_size;
extern int arg_count;

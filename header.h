#include <ctype.h>
#include <errno.h>
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
  TK_INT,
  TK_CHAR,
  TK_ENUM,
  TK_STRUCT,
  TK_EOF,
  TK_SIZEOF,
  TK_STRING
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
  INT,
  PTR,
  ARRAY,
  CHAR,
  STRUCT,
} TypeKind;

typedef struct StructField StructField;
typedef struct Type Type;
struct StructField
{
  StructField *next;
  char *name;
  int len;
  Type *type;
  int offset;
};

typedef struct StructType StructType;
struct StructType
{
  StructType *next;
  char *name;
  int len;
  StructField *fields;
  int size;
};

struct Type
{
  TypeKind ty;
  Type *ptr_to;
  size_t array_size;
  StructType *struct_type;
};

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

typedef enum
{
  EXT_FUNC,
  EXT_FUNCDECL,
  EXT_GVAR,
  EXT_ENUM,
  EXT_STRUCT,
} ExternalKind;

typedef struct External External;
struct External
{
  ExternalKind kind;
  char *name;
  int len;
  Node *code[100];
  int offsets[6];
  int size;
  StringLiteral *literals;
  int stack_size;
};

// main.c
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// codegen.c
void gen(Node *node);

// parse.c
bool at_eof();
External *external();

// tokenize.c
extern Token *token;
Token *tokenize(char *p);
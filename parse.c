#include <stdbool.h>
#include <string.h>
#include "error.h"
#include "header.h"
#include "parse.h"
#include "tokenize.h"
#include "type.h"

void assert(bool flag)
{
  if (!flag)
    error_at(token->str, "assertion failed");
}

typedef struct LVar LVar;

struct LVar
{
  LVar *next;
  char *name;
  int len;
  int offset;
  Type *type;
};

LVar *locals;

LVar *find_lvar(Token *tok)
{
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;

  return NULL;
}

LVar *new_lvar(char *name, int len, Type *type)
{
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = name;
  lvar->len = len;
  lvar->type = type;

  if (locals)
    lvar->offset = locals->offset + sizeof_type(type);
  else
    lvar->offset = sizeof_type(type);

  locals = lvar;
  return lvar;
}

typedef struct EnumVal EnumVal;
struct EnumVal
{
  EnumVal *next;
  char *name;
  int len;
  int val;
};
EnumVal *enumVals;

EnumVal *find_enum_val(Token *tok)
{
  for (EnumVal *val = enumVals; val; val = val->next)
    if (val->len == tok->len && !memcmp(tok->str, val->name, val->len))
      return val;

  return NULL;
}

EnumVal *new_enum_val(char *name, int len, int val)
{
  EnumVal *enumVal = calloc(1, sizeof(EnumVal));
  enumVal->next = enumVals;
  enumVal->name = name;
  enumVal->len = len;
  enumVal->val = val;

  enumVals = enumVal;
  return enumVal;
}

StructType *structs;

StructType *find_struct(Token *tok)
{
  for (StructType *type = structs; type; type = type->next)
    if (type->len == tok->len && !memcmp(tok->str, type->name, type->len))
      return type;

  return NULL;
}

StructField *find_struct_field(Token *tok, StructType *type)
{
  for (StructField *field = type->fields; field; field = field->next)
    if (field->len == tok->len && !memcmp(tok->str, field->name, field->len))
      return field;

  return NULL;
}

typedef struct GVar GVar;
struct GVar
{
  GVar *next;
  char *name;
  int len;
  Type *type;
};

GVar *globals;

void new_gvar(char *name, int len, Type *type)
{
  GVar *gvar = calloc(1, sizeof(GVar));
  gvar->next = globals;
  gvar->name = name;
  gvar->len = len;
  gvar->type = type;

  globals = gvar;
}

GVar *find_gvar(Token *tok)
{
  for (GVar *var = globals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;

  return NULL;
}

bool consume(char *op)
{
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;

  token = token->next;
  return true;
}

Token *consume_kind(TokenKind kind)
{
  if (token->kind == kind)
  {
    Token *tok = token;
    token = token->next;
    return tok;
  }
  return NULL;
}

Token *consume_ident()
{
  if (token->kind != TK_IDENT)
    return NULL;

  Token *res = token;
  token = token->next;
  return res;
}

void expect(char *op)
{
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "token mismatch: expected %s", op);

  token = token->next;
}

void expect_kind(TokenKind kind)
{
  if (token->kind != kind)
    error_at(token->str, "token mismatch");

  token = token->next;
}

int expect_number()
{
  if (token->kind != TK_NUM)
    error_at(token->str, "token mismatch: expected number");

  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof()
{
  return token->kind == TK_EOF;
}

typedef struct ConsumeTypeRes ConsumeTypeRes;
struct ConsumeTypeRes
{
  Type *type;
  Token *tok;
};

ConsumeTypeRes *expect_nested_type(Type *type)
{
  while (consume("*"))
    type = new_type(PTR, type);

  ConsumeTypeRes *res;

  if (consume("("))
  {
    res = expect_nested_type(type);
    expect(")");
  }
  else
  {
    Token *tok = consume_ident();
    if (!tok)
      error_at(token->str, "expected identifier");

    res = calloc(1, sizeof(ConsumeTypeRes));
    res->type = type;
    res->tok = tok;
  }

  while (consume("["))
  {
    int size = expect_number();
    expect("]");
    res->type = new_type(ARRAY, type);
    res->type->array_size = size;
  }

  return res;
}

ConsumeTypeRes *consume_type()
{
  Type *type;
  if (consume_kind(TK_INT))
    type = new_type(INT, NULL);
  else if (consume_kind(TK_CHAR))
    type = new_type(CHAR, NULL);
  else if (consume_kind(TK_STRUCT))
  {
    Token *tok = consume_ident();
    if (!tok)
      error_at(token->str, "expected identifier");

    StructType *struct_type = find_struct(tok);
    if (!struct_type)
      error_at(token->str, "unknown struct type");

    type = new_type(STRUCT, NULL);
    type->struct_type = struct_type;
  }
  else
    return NULL;

  return expect_nested_type(type);
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;

  switch (kind)
  {
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_ASSIGN:
    assert(lhs->type);
    node->type = lhs->type;
    break;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
    node->type = new_type(INT, NULL);
    break;
  case ND_ADDR:
    assert(!rhs);
    node->type = new_type(PTR, lhs->type);
    break;
  case ND_DEREF:
    assert(lhs->type->ty == PTR);
    assert(!rhs);
    node->type = lhs->type->ptr_to;
    break;
  case ND_STRING:
    node->type = new_type(PTR, new_type(CHAR, NULL));
    break;
  case ND_CALL:
    node->type = new_type(INT, NULL);
    break;
  case ND_LVAR:
  case ND_GVAR:
    error_at(token->str, "internal error");
    break;
  default:
    break;
  }

  return node;
}

Node *new_typed_node(NodeKind kind, Node *lhs, Node *rhs, Type *type)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->type = type;
  return node;
}

Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  Type *type = calloc(1, sizeof(Type));
  type->ty = INT;
  node->type = type;
  return node;
}

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *postfix();
Node *primary();

External *ext;
int literal_count;

External *external()
{
  External *external = calloc(1, sizeof(External));
  ext = external;
  int i = 0;

  if (consume_kind(TK_ENUM))
  {
    ext->kind = EXT_ENUM;

    Token *tok = consume_ident();
    if (!tok)
      error_at(token->str, "expected identifier");

    expect("{");

    tok = consume_ident();
    if (!tok)
      error_at(token->str, "expected identifier");

    new_enum_val(tok->str, tok->len, i++);

    while (consume(","))
    {
      tok = consume_ident();
      if (!tok)
        error_at(token->str, "expected identifier");

      new_enum_val(tok->str, tok->len, i++);
    }

    expect("}");
    expect(";");
    return ext;
  }

  if (consume_kind(TK_STRUCT))
  {
    ext->kind = EXT_STRUCT;
    Token *tok = consume_ident();
    if (!tok)
      error_at(token->str, "expected identifier");

    StructType *strType = calloc(1, sizeof(StructType));
    strType->name = tok->str;
    strType->len = tok->len;

    expect("{");
    ConsumeTypeRes *res = consume_type();
    StructField *field = calloc(1, sizeof(StructField));
    strType->fields = field;

    field->name = res->tok->str;
    field->len = res->tok->len;
    field->type = res->type;
    field->offset = sizeof_type(field->type);

    while (consume(";"))
    {
      res = consume_type();
      if (!res)
        break;
      field->next = calloc(1, sizeof(StructField));
      field->next->name = res->tok->str;
      field->next->len = res->tok->len;
      field->next->type = res->type;
      field->next->offset = field->offset + sizeof_type(field->next->type);
      field = field->next;
    }
    strType->size = field->offset;

    expect("}");
    expect(";");

    strType->next = structs;
    structs = strType;
  }

  ConsumeTypeRes *res = consume_type();
  if (!res)
    error_at(token->str, "invalid type");

  external->name = res->tok->str;
  external->len = res->tok->len;
  if (consume("("))
  {
    external->kind = EXT_FUNC;
    if (!consume(")"))
    {
      for (;;)
      {
        expect_kind(TK_INT);
        Token *arg = consume_ident();
        if (!arg)
          error_at(res->tok->str, "failed to parse argument");

        LVar *lvar = find_lvar(arg);
        if (!lvar)
          lvar = new_lvar(arg->str, arg->len, new_type(INT, NULL));

        external->offsets[i] = lvar->offset;
        i++;

        if (!consume(","))
          break;
      }

      expect(")");
    }

    if (consume(";"))
    {
      external->kind = EXT_FUNCDECL;
      return external;
    }

    expect("{");

    i = 0;

    while (!consume("}"))
      external->code[i++] = stmt();
    external->code[i] = NULL;
  }
  else
  {
    external->kind = EXT_GVAR;

    new_gvar(res->tok->str, res->tok->len, res->type);
    external->size = sizeof_type(res->type);

    expect(";");
  }
  if (locals)
    external->stack_size = locals->offset;

  return external;
}

Node *stmt()
{
  Node *node;

  if (consume_kind(TK_IF))
  {
    expect("(");
    Node *lhs = expr();
    expect(")");
    Node *rhs = stmt();

    if (consume_kind(TK_ELSE))
      node = new_node(ND_IFELSE, lhs, new_node(ND_UNNAMED, rhs, stmt()));
    else
      node = new_node(ND_IF, lhs, rhs);
  }
  else if (consume_kind(TK_WHILE))
  {
    expect("(");
    Node *lhs = expr();
    expect(")");
    node = new_node(ND_WHILE, lhs, stmt());
  }
  else if (consume_kind(TK_FOR))
  {
    Node *a, *b, *c, *d;
    expect("(");
    if (!consume(";"))
    {
      a = expr();
      expect(";");
    }
    if (!consume(";"))
    {
      b = expr();
      expect(";");
    }
    if (!consume(")"))
    {
      c = expr();
      expect(")");
    }
    d = stmt();

    Node *lhs = new_node(ND_UNNAMED, a, b);
    Node *rhs = new_node(ND_UNNAMED, c, d);

    node = new_node(ND_FOR, lhs, rhs);
  }
  else if (consume("{"))
  {
    node = new_node(ND_BLOCK, NULL, NULL);

    for (Node *last = node; !consume("}"); last = last->rhs)
      last->rhs = new_node(ND_UNNAMED, stmt(), NULL);
  }
  else if (consume_kind(TK_RETURN))
  {
    node = new_node(ND_RETURN, expr(), NULL);
    expect(";");
  }
  else
  {
    node = expr();
    expect(";");
  }

  return node;
}

Node *expr()
{
  return assign();
}

Node *assign()
{
  Node *node = equality();
  if (consume("="))
  {
    Node *r = assign();
    node = new_node(ND_ASSIGN, node, r);
  }
  return node;
}

Node *equality()
{
  Node *node = relational();

  for (;;)
  {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational()
{
  Node *node = add();

  for (;;)
  {
    if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else
      return node;
  }
}

Node *add()
{
  Node *node = mul();

  for (;;)
  {
    if (node->type->ty == PTR || node->type->ty == ARRAY)
    {
      int size = sizeof_type(node->type->ptr_to);

      if (consume("+"))
      {
        if (node->type->ty == ARRAY)
          node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
        node = new_node(ND_ADD, node, new_node(ND_MUL, mul(), new_node_num(size)));
      }
      else if (consume("-"))
      {
        if (node->type->ty == ARRAY)
          node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
        node = new_node(ND_SUB, node, new_node(ND_MUL, mul(), new_node_num(size)));
      }
      return node;
    }

    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul()
{
  Node *node = unary();

  for (;;)
  {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary()
{
  if (consume("+"))
    return postfix();
  else if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), postfix());
  else if (consume("&"))
    return new_node(ND_ADDR, primary(), NULL);
  else if (consume("*"))
  {
    Node *l = primary();
    if (l->type->ty != PTR && l->type->ty != ARRAY)
      error_at(token->str, "dereference failed: not a pointer");

    if (l->type->ty == ARRAY)
      l = new_node(ND_ADDR, l, NULL);

    return new_node(ND_DEREF, l, NULL);
  }
  else if (consume_kind(TK_SIZEOF))
    return new_node_num(sizeof_type(unary()->type));

  return postfix();
}

Node *postfix()
{
  Node *node = primary();

  for (;;)
  {
    if (consume("["))
    {
      Node *subscript = expr();
      int size = sizeof_type(node->type->ptr_to);
      if (node->type->ty == ARRAY)
        node = new_typed_node(ND_ADDR, node, NULL, new_type(PTR, node->type->ptr_to));
      node = new_node(ND_DEREF, new_node(ND_ADD, node, new_node(ND_MUL, subscript, new_node_num(size))), NULL);
      expect("]");
      continue;
    }
    if (consume("."))
    {
      Token *tok = consume_ident();
      if (!tok)
        error_at(token->str, "expected identifier after '.'");

      if (node->type->ty != STRUCT)
        error_at(token->str, "expected struct type");

      int size = node->type->struct_type->size;
      StructField *field = find_struct_field(tok, node->type->struct_type);
      if (!field)
        error_at(token->str, "no such field");

      node = new_node(ND_ADDR, node, NULL);
      node = new_node(ND_ADD, node, new_node_num(size - field->offset));
      node->type = new_type(PTR, field->type);
      node = new_node(ND_DEREF, node, NULL);
      continue;
    }
    if (consume("->"))
    {
      Token *tok = consume_ident();
      if (!tok)
        error_at(token->str, "expected identifier after '->'");

      if (node->type->ty != PTR)
        error_at(token->str, "expected pointer type");

      if (node->type->ptr_to->ty != STRUCT)
        error_at(token->str, "expected struct type");

      StructField *field = find_struct_field(tok, node->type->ptr_to->struct_type);
      if (!field)
        error_at(token->str, "no such field");

      node = new_node(ND_ADD, node, new_node_num(node->type->ptr_to->struct_type->size - field->offset));
      node->type = new_type(PTR, field->type);
      node = new_node(ND_DEREF, node, NULL);
      continue;
    }

    break;
  }
  return node;
}

Node *primary()
{
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_kind(TK_STRING);
  if (tok)
  {
    StringLiteral *s = calloc(1, sizeof(StringLiteral));
    s->str = tok->str;
    s->len = tok->len;
    s->next = ext->literals;
    ext->literals = s;
    Node *node = new_node(ND_STRING, NULL, NULL);
    node->offset = literal_count++;
    s->offset = node->offset;
    return node;
  }

  ConsumeTypeRes *res = consume_type();
  if (res)
  {
    LVar *lvar = find_lvar(res->tok);
    if (!lvar)
      lvar = new_lvar(res->tok->str, res->tok->len, res->type);

    Node *node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
    node->offset = lvar->offset;

    if (consume("="))
    {
      if (consume("{"))
      {
        if (lvar->type->ty != ARRAY)
          error_at(tok->str, "type mismatch");

        node = new_typed_node(ND_ASSIGN_ARRAY, node, NULL, lvar->type);
        Node *last = node;

        for (int i = 0; i < lvar->type->array_size; i++)
        {
          Node *ptr = new_node(ND_ADD, new_node(ND_ADDR, node->lhs, NULL), new_node_num(i * sizeof_type(lvar->type->ptr_to)));
          Node *deref = new_node(ND_DEREF, ptr, NULL);

          last->rhs = new_node(ND_UNNAMED, new_node(ND_ASSIGN, deref, assign()), NULL);
          last = last->rhs;

          if (i != lvar->type->array_size - 1)
            expect(",");
        }
        expect("}");
      }
      else
        node = new_node(ND_ASSIGN, node, assign());
    }

    return node;
  }

  tok = consume_ident();
  if (tok)
  {
    Node *node;

    if (consume("("))
    {
      Node *func = calloc(1, sizeof(Node));
      func->name = tok->str;
      func->len = tok->len;

      node = new_node(ND_CALL, func, NULL);

      Node *last = node;
      if (consume(")"))
        return node;
      for (;;)
      {
        last->rhs = new_node(ND_UNNAMED, expr(), NULL);
        last = last->rhs;

        if (consume(")"))
          break;
        else
          expect(",");
      }
    }
    else
    {
      LVar *lvar = find_lvar(tok);
      if (lvar)
      {
        node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
        node->offset = lvar->offset;
      }
      else
      {
        EnumVal *val = find_enum_val(tok);
        if (val)
          node = new_node_num(val->val);
        else
        {
          GVar *gvar = find_gvar(tok);
          if (gvar)
          {
            node = new_typed_node(ND_GVAR, NULL, NULL, gvar->type);
            node->name = tok->str;
            node->len = tok->len;
          }
          else
            error_at(tok->str, "%.*s is not defined", tok->len, tok->str);
        }
      }
    }

    return node;
  }

  return new_node_num(expect_number());
}
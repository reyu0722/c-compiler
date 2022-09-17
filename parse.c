#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "header.h"
#include "parse.h"
#include "tokenize.h"
#include "type.h"

typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct EnumVal EnumVal;
typedef struct ConsumeTypeRes ConsumeTypeRes;
typedef struct TypeDef TypeDef;
typedef struct Enum Enum;

struct LVar
{
  LVar *next;
  String *name;
  int offset;
  Type *type;
};

struct GVar
{
  GVar *next;
  String *name;
  Type *type;
};

struct EnumVal
{
  EnumVal *next;
  String *name;
  int val;
};

struct ConsumeTypeRes
{
  Type *type;
  Token *tok;
};

struct TypeDef
{
  TypeDef *next;
  String *name;
  Type *type;
};

LVar *locals;
GVar *globals;
EnumVal *enumVals;
StructType *structs;
TypeDef *typedefs;

LVar *find_lvar(Token *tok)
{
  for (LVar *var = locals; var; var = var->next)
    if (str_equals(var->name, tok->str))
      return var;

  return NULL;
}

LVar *new_lvar(String *name, Type *type)
{
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = name;
  lvar->type = type;

  if (locals)
    lvar->offset = locals->offset + sizeof_type(type);
  else
    lvar->offset = sizeof_type(type);

  locals = lvar;
  return lvar;
}

EnumVal *find_enum_val(Token *tok)
{
  for (EnumVal *val = enumVals; val; val = val->next)
    if (str_equals(val->name, tok->str))
      return val;

  return NULL;
}

EnumVal *new_enum_val(String *name, int val)
{
  EnumVal *enumVal = calloc(1, sizeof(EnumVal));
  enumVal->next = enumVals;
  enumVal->name = name;
  enumVal->val = val;

  enumVals = enumVal;
  return enumVal;
}

StructType *find_struct(Token *tok)
{
  for (StructType *type = structs; type; type = type->next)
    if (str_equals(type->name, tok->str))
      return type;

  return NULL;
}

StructField *find_struct_field(Token *tok, StructType *type)
{
  for (StructField *field = type->fields; field; field = field->next)
    if (str_equals(field->name, tok->str))
      return field;

  return NULL;
}

void new_gvar(String *name, Type *type)
{
  GVar *gvar = calloc(1, sizeof(GVar));
  gvar->next = globals;
  gvar->name = name;
  gvar->type = type;

  globals = gvar;
}

GVar *find_gvar(Token *tok)
{
  for (GVar *var = globals; var; var = var->next)
    if (str_equals(var->name, tok->str))
      return var;

  return NULL;
}

TypeDef *find_typedef(Token *tok)
{
  for (TypeDef *type = typedefs; type; type = type->next)
    if (str_equals(type->name, tok->str))
      return type;

  return NULL;
}

TypeDef *new_typedef(String *name, Type *type)
{
  TypeDef *typedef_ = calloc(1, sizeof(TypeDef));
  typedef_->next = typedefs;
  typedef_->name = name;
  typedef_->type = type;

  typedefs = typedef_;
  return typedef_;
}

bool consume(char *op)
{
  if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op))
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

bool check_kind(TokenKind kind)
{
  return token->kind == kind;
}

void go_to(Token *tok)
{
  token = tok;
}

void expect(char *op)
{
  if (token->kind != TK_RESERVED || !str_chr_equals(token->str, op))
    error_at_here("token mismatch: expected %s", op);

  token = token->next;
}

void expect_kind(TokenKind kind)
{
  if (token->kind != kind)
    error_at_here("token mismatch");

  token = token->next;
}

int expect_number()
{
  if (token->kind != TK_NUM)
    error_at_here("token mismatch: expected number");

  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof()
{
  return token->kind == TK_EOF;
}

ConsumeTypeRes *consume_type();
Type *consume_type_name()
{
  if (consume_kind(TK_INT))
    return new_type(INT, NULL);
  if (consume_kind(TK_CHAR))
    return new_type(CHAR, NULL);
  if (consume_kind(TK_VOID))
    return new_type(VOID, NULL);
  if (consume_kind(TK_BOOL))
    return new_type(BOOL, NULL);
  if (consume_kind(TK_LONG))
  {
    if (consume_kind(TK_INT))
      return new_type(INT, NULL); // long int
    if (consume_kind(TK_UNSIGNED))
    {
      if (consume_kind(TK_INT))
        return new_type(INT, NULL); // unsigned long int
      return NULL;
    }

    return new_type(INT, NULL); // long int
  }
  if (consume_kind(TK_UNSIGNED))
  {
    if (consume_kind(TK_INT))
      return new_type(INT, NULL); // unsigned int
    if (consume_kind(TK_CHAR))
      return new_type(CHAR, NULL); // unsigned char
    if (consume_kind(TK_SHORT))
    {
      consume_kind(TK_INT);
      return new_type(INT, NULL); // unsigned short
    }
    if (consume_kind(TK_LONG))
    {
      consume_kind(TK_INT);
      return new_type(INT, NULL); // unsigned long int
    }
    return NULL;
  }
  if (consume_kind(TK_SIGNED))
  {
    if (consume_kind(TK_INT))
      return new_type(INT, NULL); // int
    if (consume_kind(TK_CHAR))
      return new_type(CHAR, NULL); // char
    if (consume_kind(TK_SHORT))
    {
      consume_kind(TK_INT);
      return new_type(INT, NULL); // short
    }
    if (consume_kind(TK_LONG))
    {
      consume_kind(TK_INT);
      return new_type(INT, NULL); // long int
    }
    return NULL;
  }
  if (consume_kind(TK_ENUM))
  {
    consume_kind(TK_IDENT);
    if (consume("{"))
    {
      int i = 0;
      while (!consume("}"))
      {
        Token *name = consume_ident();
        if (!name)
          error_at_here("expected enum name");

        new_enum_val(name->str, i);

        consume(",");
        i++;
      }
    }
    return new_type(INT, NULL);
  }

  if (check_kind(TK_STRUCT) || check_kind(TK_UNION))
  {
    bool is_union = false;
    if (consume_kind(TK_UNION))
      is_union = true;
    else
      consume_kind(TK_STRUCT);

    Token *id = consume_kind(TK_IDENT);

    if (consume("{"))
    {
      String *name = NULL;
      if (id)
        name = id->str;
      Type *type = new_struct_type(name, is_union);

      while (!consume("}"))
      {
        ConsumeTypeRes *res = consume_type();
        if (!res)
          error_at_here("expected type");

        add_field(type->struct_type, res->type, res->tok->str);
        expect(";");
      }

      if (id)
      {
        StructType *s = find_struct(id);
        if (s)
        {
          if (s->fields)
            error_at_here("struct %.*s is already defined", id->str->len, id->str->ptr);
          else
          {
            s->fields = type->struct_type->fields;
            s->is_union = type->struct_type->is_union;
            s->alignment = type->struct_type->alignment;
          }
        }
        else
        {
          type->struct_type->next = structs;
          structs = type->struct_type;
        }
      }

      return type;
    }
    else
    {
      if (!id)
        error_at_here("expected struct name");
      StructType *type = find_struct(id);
      Type *ty;
      if (!type)
      {
        // forward declaration
        ty = new_struct_type(id->str, is_union);
        ty->struct_type->next = structs;
        structs = ty->struct_type;
      }
      else
      {
        ty = new_type(STRUCT, NULL);
        ty->struct_type = type;
      }
      return ty;
    }
  }

  Token *tok = consume_kind(TK_IDENT);
  if (tok)
  {
    TypeDef *type = find_typedef(tok);
    if (type)
      return type->type;
    go_to(tok);
  }

  return NULL;
}

Node *expr();
Type *consume_noident_type();
ConsumeTypeRes *expect_nested_type(Type *type)
{
  int ptr = 0;
  ConsumeTypeRes *res;

  if (consume("("))
  {
    while (consume("*"))
      ptr++;
    res = expect_nested_type(type);
    expect(")");
  }
  else
  {
    Token *tok = consume_ident();
    if (!tok)
      error_at_here("expected identifier");

    res = calloc(1, sizeof(ConsumeTypeRes));
    res->type = type;
    res->tok = tok;
  }
  if (consume("("))
  {
    consume_noident_type();
    consume_ident();
    while (consume(","))
    {
      assert(consume_noident_type() != NULL);
      consume_ident();
    }

    expect(")");

    res->type = new_type(FUNC, NULL);
  }
  for (int i = 0; i < ptr; i++)
    res->type = new_type(PTR, res->type);

  while (consume("["))
  {
    Node *size = expr();
    if (size->kind != ND_NUM)
      error_at_here("expected constant expression");
    expect("]");
    res->type = new_type(ARRAY, type);
    res->type->array_size = size->val;
  }

  return res;
}

ConsumeTypeRes *consume_type()
{
  Type *type = consume_type_name();
  if (!type)
    return NULL;
  while (consume("*"))
    type = new_type(PTR, type);
  ConsumeTypeRes *res = expect_nested_type(type);
  return res;
}

Type *expect_noident_type(Type *type)
{
  while (consume("*"))
    type = new_type(PTR, type);

  if (consume("("))
  {
    type = expect_noident_type(type);
    expect(")");
  }

  if (consume("("))
  {
    consume_noident_type();
    while (consume(","))
      consume_noident_type();
    expect(")");
    type = new_type(FUNC, NULL);
  }

  while (consume("["))
  {
    Node *size = expr();
    if (size->kind != ND_NUM)
      error_at_here("expected constant expression");
    expect("]");
    type = new_type(ARRAY, type);
    type->array_size = size->val;
  }

  return type;
}

Type *consume_noident_type()
{
  Type *type = consume_type_name();
  if (!type)
    return NULL;

  return expect_noident_type(type);
}

void next()
{
  token = token->next;
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
  case ND_AND:
  case ND_OR:
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
    error_at_here("internal error");
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

Node *new_node_char(char val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  Type *type = calloc(1, sizeof(Type));
  type->ty = CHAR;
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
  locals = NULL;

  External *external = calloc(1, sizeof(External));
  ext = external;
  int i = 0;

  if (check_kind(TK_ENUM))
  {
    ext->kind = EXT_ENUM;

    consume_type_name();
    expect(";");
    return ext;
  }

  if (check_kind(TK_STRUCT) || check_kind(TK_UNION))
  {
    ext->kind = EXT_STRUCT;
    Type *ty = consume_type_name();
    if (!ty)
      error_at_here("expected struct or union");
    expect(";");
    return ext;
  }

  if (consume_kind(TK_TYPEDEF))
  {
    ext->kind = EXT_TYPEDEF;

    ConsumeTypeRes *res = consume_type();
    if (!res)
      error_at_here("expected type");

    new_typedef(res->tok->str, res->type);
    expect(";");
    return ext;
  }

  ConsumeTypeRes *res = consume_type();
  if (!res)
    error_at_here("invalid type");

  external->name = res->tok->str;
  if (res->type->ty == FUNC)
  {
    external->kind = EXT_FUNC;
    go_to(res->tok);
    next();
    expect("(");

    Token *cur = token;
    bool no_args = false;
    if (consume_kind(TK_VOID))
    {
      if (consume(")"))
        no_args = true;
      else
        go_to(cur);
    }
    if (!consume(")") && !no_args)
    {
      for (;;)
      {
        ConsumeTypeRes *res = consume_type();
        if (!res)
          error_at_here("failed to parse argument");

        LVar *lvar = find_lvar(res->tok);
        if (!lvar)
          lvar = new_lvar(res->tok->str, res->type);

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

    new_gvar(res->tok->str, res->type);
    external->size = sizeof_type(res->type);

    expect(";");
  }
  if (locals)
    external->stack_size = (locals->offset / 8) * 8 + 8;

  return external;
}

Node *stmt()
{
  Node *node;

  if (consume_kind(TK_CASE))
  {
    Node *e = expr();
    if (e->kind != ND_NUM)
      error_at_here("expected constant expression");

    expect(":");
    node = new_node(ND_CASE, e, stmt());
  }
  else if (consume_kind(TK_IF))
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
  else if (consume_kind(TK_SWITCH))
  {
    expect("(");
    Node *lhs = expr();
    expect(")");
    Node *rhs = stmt();

    node = new_node(ND_SWITCH, lhs, rhs);
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
  else if (consume_kind(TK_BREAK))
  {
    node = new_node(ND_BREAK, NULL, NULL);
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
    node = new_node(ND_ASSIGN, node, assign());
  else if (consume("+="))
    node = new_node(ND_ASSIGN, node, new_node(ND_ADD, node, assign()));
  else if (consume("-="))
    node = new_node(ND_ASSIGN, node, new_node(ND_SUB, node, assign()));
  else if (consume("*="))
    node = new_node(ND_ASSIGN, node, new_node(ND_MUL, node, assign()));
  else if (consume("/="))
    node = new_node(ND_ASSIGN, node, new_node(ND_DIV, node, assign()));

  return node;
}

Node *equality()
{
  Node *node = relational();

  for (;;)
  {
    if (consume("||"))
      node = new_node(ND_OR, node, relational());
    else if (consume("&&"))
      node = new_node(ND_AND, node, relational());
    else if (consume("=="))
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
    {
      Node *rhs = mul();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val += rhs->val;
      else
        node = new_node(ND_ADD, node, rhs);
    }
    else if (consume("-"))
    {
      Node *rhs = mul();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val -= rhs->val;
      else
        node = new_node(ND_SUB, node, rhs);
    }
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
    {
      Node *rhs = unary();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val *= rhs->val;
      else
        node = new_node(ND_MUL, node, rhs);
    }
    else if (consume("/"))
    {
      Node *rhs = unary();
      if (node->kind == ND_NUM && rhs->kind == ND_NUM)
        node->val /= rhs->val;
      else
        node = new_node(ND_DIV, node, rhs);
    }
    else
      return node;
  }
}

Node *unary()
{
  if (consume("+"))
    return postfix();
  else if (consume("-"))
  {
    Node *rhs = postfix();
    if (rhs->kind == ND_NUM)
    {
      rhs->val = -rhs->val;
      return rhs;
    }
    else
      return new_node(ND_SUB, new_node_num(0), rhs);
  }
  else if (consume("&"))
    return new_node(ND_ADDR, postfix(), NULL);
  else if (consume("*"))
  {
    Node *l = primary();
    if (l->type->ty != PTR && l->type->ty != ARRAY)
      error_at_here("dereference failed: not a pointer");

    if (l->type->ty == ARRAY)
      l = new_node(ND_ADDR, l, NULL);

    return new_node(ND_DEREF, l, NULL);
  }
  else if (consume_kind(TK_SIZEOF))
  {
    Token *cur = token;
    if (consume("("))
    {
      Type *type = consume_noident_type();
      if (type)
      {
        expect(")");
        return new_node_num(sizeof_type(type));
      }
      go_to(cur);
    }

    return new_node_num(sizeof_type(unary()->type));
  }
  else if (consume("!"))
    return new_node(ND_EQ, postfix(), new_node_num(0));

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
        error_at_token(tok, "expected identifier after '.'");

      if (node->type->ty != STRUCT)
        error_at_token(tok, "expected struct type");

      StructField *field = find_struct_field(tok, node->type->struct_type);
      if (!field)
        error_at_here("no such field");

      node = new_node(ND_ADDR, node, NULL);
      node = new_node(ND_ADD, node, new_node_num(field->index * node->type->ptr_to->struct_type->alignment));
      node->type = new_type(PTR, field->type);
      node = new_node(ND_DEREF, node, NULL);
      continue;
    }
    if (consume("->"))
    {
      Token *tok = consume_ident();
      if (!tok)
        error_at_token(tok, "expected identifier after '->'");

      if (node->type->ty != PTR)
        error_at_token(tok, "expected pointer type");

      if (node->type->ptr_to->ty != STRUCT)
        error_at_token(tok, "expected struct type");

      StructField *field = find_struct_field(tok, node->type->ptr_to->struct_type);
      if (!field)
        error_at_here("no such field");

      node = new_node(ND_ADD, node, new_node_num(field->index * node->type->ptr_to->struct_type->alignment));
      node->type = new_type(PTR, field->type);
      node = new_node(ND_DEREF, node, NULL);
      continue;
    }
    if (consume("++"))
    {
      node = new_node(ND_ASSIGN, node, new_node(ND_ADD, node, new_node_num(1)));
      continue;
    }
    if (consume("--"))
    {
      node = new_node(ND_ASSIGN, node, new_node(ND_SUB, node, new_node_num(1)));
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
      lvar = new_lvar(res->tok->str, res->type);

    Node *node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
    node->offset = lvar->offset;

    if (consume("="))
    {
      if (consume("{"))
      {
        if (lvar->type->ty != ARRAY)
          error_at_token(tok, "type mismatch");

        node = new_typed_node(ND_ASSIGN_ARRAY, node, NULL, lvar->type);
        Node *last = node;

        for (int i = 0; i < lvar->type->array_size; i++)
        {
          Node *ptr = new_typed_node(ND_ADD, new_node(ND_ADDR, node->lhs, NULL), new_node_num(i * sizeof_type(lvar->type->ptr_to)), new_type(PTR, lvar->type->ptr_to));
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
          }
          else
            error_at_token(tok, "%.*s is not defined", tok->str->len, tok->str->ptr);
        }
      }
    }

    return node;
  }

  tok = consume_kind(TK_CHAR_CONST);
  if (tok)
  {
    Node *node = new_node_char(*tok->str->ptr);
    return node;
  }

  return new_node_num(expect_number());
}
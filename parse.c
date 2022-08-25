#include "mycc.h"

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
    lvar->offset = locals->offset + 8;
  else
    lvar->offset = 8;

  locals = lvar;
  return lvar;
}

bool consume(char *op)
{
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;

  token = token->next;
  return true;
}

bool consume_kind(TokenKind kind)
{
  if (token->kind == kind)
  {
    token = token->next;
    return true;
  }
  return false;
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_typed_node(NodeKind kind, Node *lhs, Node *rhs, Type *type)
{
  Node *node = new_node(kind, lhs, rhs);
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
Node *primary();

Function function;

void parse_function()
{
  int i = 0;

  expect_kind(TK_INT);
  Token *tok = consume_ident();
  function.name = tok->str;
  function.len = tok->len;
  expect("(");

  if (!consume(")"))
  {
    for (;;)
    {
      expect_kind(TK_INT);
      Token *arg = consume_ident();
      if (!arg)
        error_at(tok->str, "failed to parse argument");

      LVar *lvar = find_lvar(arg);
      Type *type = calloc(1, sizeof(Type));
      type->ty = INT;

      if (!lvar)
        lvar = new_lvar(arg->str, arg->len, type);

      function.offsets[i] = lvar->offset;
      i++;

      if (!consume(","))
        break;
    }

    expect(")");
  }

  expect("{");

  i = 0;

  while (!consume("}"))
    function.code[i++] = stmt();
  function.code[i] = NULL;
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
    node = new_typed_node(ND_ASSIGN, node, r, r->type);
  }
  return node;
}

Node *equality()
{
  Node *node = relational();

  Type *type = calloc(1, sizeof(Type));
  type->ty = INT;

  for (;;)
  {
    if (consume("=="))
      node = new_typed_node(ND_EQ, node, relational(), type);
    else if (consume("!="))
      node = new_typed_node(ND_NE, node, relational(), type);
    else
      return node;
  }
}

Node *relational()
{
  Node *node = add();

  Type *type = calloc(1, sizeof(Type));
  type->ty = INT;

  for (;;)
  {
    if (consume("<="))
      node = new_typed_node(ND_LE, node, add(), type);
    else if (consume("<"))
      node = new_typed_node(ND_LT, node, add(), type);
    else if (consume(">="))
      node = new_typed_node(ND_LE, add(), node, type);
    else if (consume(">"))
      node = new_typed_node(ND_LT, add(), node, type);
    else
      return node;
  }
}

Node *add()
{
  Node *node = mul();

  for (;;)
  {
    if (node->kind == ND_LVAR && node->type->ty == PTR)
    {
      int size;
      if (node->type->ptr_to->ty == INT)
        size = 4;
      else
        size = 8;

      if (consume("+"))
        node = new_typed_node(ND_ADD, node, new_node(ND_MUL, mul(), new_node_num(size)), node->type);
      else if (consume("-"))
        node = new_typed_node(ND_SUB, node, new_node(ND_MUL, mul(), new_node_num(size)), node->type);
      else
        return node;
    }

    if (consume("+"))
      node = new_typed_node(ND_ADD, node, mul(), node->type);
    else if (consume("-"))
      node = new_typed_node(ND_SUB, node, mul(), node->type);
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
      node = new_typed_node(ND_MUL, node, unary(), node->type);
    else if (consume("/"))
      node = new_typed_node(ND_DIV, node, unary(), node->type);
    else
      return node;
  }
}

Node *unary()
{
  if (consume("+"))
    return primary();
  else if (consume("-"))
  {
    Type *type = calloc(1, sizeof(Type));
    type->ty = INT;
    return new_typed_node(ND_SUB, new_node_num(0), primary(), type);
  }
  else if (consume("&"))
  {
    Node *l = primary();

    Type *type = calloc(1, sizeof(Type));
    type->ty = PTR;
    type->ptr_to = l->type;

    return new_typed_node(ND_ADDR, l, NULL, type);
  }
  else if (consume("*"))
  {
    Node *l = primary();
    if (l->type->ty != PTR)
      error_at(token->str, "dereference failed: not a pointer");
    return new_typed_node(ND_DEREF, l, NULL, l->type->ptr_to);
  }
  else
    return primary();
}

Node *primary()
{
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }

  if (consume_kind(TK_INT))
  {
    Type *type = calloc(1, sizeof(Type));
    type->ty = INT;

    while (consume("*"))
    {
      Type *new_type = calloc(1, sizeof(Type));
      new_type->ty = PTR;
      new_type->ptr_to = type;
      type = new_type;
    }

    Token *tok = consume_ident();

    if (!tok)
      error_at(token->str, "parse failed");

    LVar *lvar = find_lvar(tok);
    if (!lvar)
      lvar = new_lvar(tok->str, tok->len, type);

    Node *node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
    node->offset = lvar->offset;
    return node;
  }

  Token *tok = consume_ident();
  if (tok)
  {
    Node *node;

    if (consume("("))
    {
      Node *func = calloc(1, sizeof(Node));
      func->name = tok->str;
      func->len = tok->len;

      Type *type = calloc(1, sizeof(Type));
      type->ty = INT;
      node = new_typed_node(ND_CALL, func, NULL, type);

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
      if (!lvar)
        error_at(tok->str, "%.*s is not defined", tok->len, tok->str);

      node = new_typed_node(ND_LVAR, NULL, NULL, lvar->type);
      node->offset = lvar->offset;
    }

    return node;
  }

  return new_node_num(expect_number());
}
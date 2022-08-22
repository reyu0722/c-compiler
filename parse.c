#include "mycc.h"

typedef struct LVar LVar;

struct LVar
{
  LVar *next;
  char *name;
  int len;
  int offset;
};

LVar *locals;

LVar *find_lvar(Token *tok)
{
  for (LVar *var = locals; var; var = var->next)
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

void expect(char op)
{
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error_at(token->str, "token mismatch: expected %c", op);

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

Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
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

Node *stmt()
{
  Node *node;

  if (consume_kind(TK_IF))
  {
    expect('(');
    Node *lhs = expr();
    expect(')');
    Node *rhs = stmt();

    if (consume_kind(TK_ELSE))
    {
      rhs = new_node(ND_UNNAMED, rhs, stmt());
      node = new_node(ND_IFELSE, lhs, rhs);
    }
    else
    {
      node = new_node(ND_IF, lhs, rhs);
    }

    return node;
  }

  if (consume_kind(TK_WHILE))
  {
    expect('(');
    Node *lhs = expr();
    expect(')');
    node = new_node(ND_WHILE, lhs, stmt());
    return node;
  }

  if (consume_kind(TK_FOR))
  {
    Node *a, *b, *c, *d;
    expect('(');
    if (!consume(";"))
    {
      a = expr();
      expect(';');
    }
    if (!consume(";"))
    {
      b = expr();
      expect(';');
    }
    if (!consume(")"))
    {
      c = expr();
      expect(')');
    }
    d = stmt();

    Node *lhs = new_node(ND_UNNAMED, a, b);
    Node *rhs = new_node(ND_UNNAMED, c, d);

    node = new_node(ND_FOR, lhs, rhs);
    return node;
  }

  if (consume("{"))
  {
    node = new_node(ND_BLOCK, NULL, NULL);
    Node *last = node;
    while (!consume("}"))
    {
      last->rhs = new_node(ND_UNNAMED, stmt(), NULL);
      last = last->rhs;
    }

    return node;
  }

  if (consume_kind(TK_RETURN))
  {
    node = new_node(ND_RETURN, expr(), NULL);
  }
  else
  {
    node = expr();
  }

  expect(';');
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
    node = new_node(ND_ASSIGN, node, assign());
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
    return primary();
  else if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  else
    return primary();
}

Node *primary()
{
  if (consume("("))
  {
    Node *node = expr();
    expect(')');
    return node;
  }

  Token *tok = consume_ident();
  if (tok)
  {
    if (consume("("))
    {
      expect(')');

      Node *func = calloc(1, sizeof(Node));
      func->name = tok->str;
      func->len = tok->len;

      Node *node = new_node(ND_CALL, func, NULL);
      return node;
    }

    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar)
    {
      node->offset = lvar->offset;
    }
    else
    {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals)
        lvar->offset = locals->offset + 8;
      else
        lvar->offset = 0;

      node->offset = lvar->offset;
      locals = lvar;
    }

    return node;
  }

  return new_node_num(expect_number());
}

Node *code[100];

void program()
{
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}
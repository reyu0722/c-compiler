#include <stdio.h>
#include "codegen.h"
#include "error.h"
#include "header.h"

int label_count = 0;
int switch_count = 0;
int break_count = 0;
int continue_count = 0;

void gen_lval(Node *node)
{
  if (node->kind == ND_DEREF)
    gen(node->lhs);
  else if (node->kind == ND_LVAR)
  {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
  }
  else if (node->kind == ND_GVAR)
  {
    printf("  lea rax, %.*s[rip]\n", node->name->len, node->name->ptr);
    printf("  push rax\n");
  }
  else if (node->kind == ND_STRING)
  {
    printf("  lea rax, .LC%d[rip]\n", node->offset);
    printf("  push rax\n");
  }
  else
    error("left value of assignment must be variable: found %d\n", node->kind);
}

void gen(Node *node)
{
  int l, i;
  Node *n;

  switch (node->kind)
  {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
  case ND_GVAR:
    gen_lval(node);
    printf("  pop rax\n");
    switch (sizeof_type(node->type))
    {
    case 4:
      printf("  mov eax, [rax]\n");
      break;
    case 8:
      printf("  mov rax, [rax]\n");
      break;
    case 1:
      printf("  movsx rax, BYTE PTR [rax]\n");
      break;
    default:
      break;
    }
    printf("  push rax\n");
    return;
  case ND_STRING:
    gen_lval(node);
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (sizeof_type(node->lhs->type))
    {
    case 4:
      printf("  mov [rax], edi\n");
      break;
    case 1:
      printf("  mov [rax], dil\n");
      break;
    case 8:
      printf("  mov [rax], rdi\n");
      break;
    default:
      error("failed to assign");
    }

    printf("  push rdi\n");
    return;
  case ND_ASSIGN_ARRAY:
    for (node = node->rhs; node; node = node->rhs)
    {
      gen(node->lhs);
      printf("  pop rax\n");
    }
    printf("  push 0\n");
    return;
  case ND_RETURN:
    if (node->lhs)
      gen(node->lhs);
    else
      printf("  push 0\n");
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_IF:
    l = label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l);
    gen(node->rhs);
    printf("  pop rax\n");
    printf(".Lend%d:\n", l);
    printf("  push 0\n");
    return;
  case ND_IFELSE:
    l = label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", l);
    gen(node->rhs->lhs);
    printf("  pop rax\n");
    printf("  jmp .Lend%d\n", l);
    printf(".Lelse%d:\n", l);
    gen(node->rhs->rhs);
    printf("  pop rax\n");
    printf(".Lend%d:\n", l);
    printf("  push 0\n");
    return;
  case ND_SWITCH:
    assert(node->rhs->kind == ND_BLOCK);
    gen(node->lhs);
    printf("  pop rax\n");

    for (n = node->rhs; n; n = n->rhs)
    {
      if (n->lhs && n->lhs->kind == ND_CASE)
      {
        printf("  cmp rax, %d\n", n->lhs->lhs->val);
        printf("  je .Lcase%d_%d\n", switch_count, n->lhs->lhs->val);
      }
      if (n->lhs && n->lhs->kind == ND_DEFAULT)
        printf("  jmp .Ldefault%d\n", switch_count);
    }

    int b = break_count;
    gen(node->rhs);
    printf(".Lbreak%d:\n", b);
    printf("  push 0\n");
    break_count++;
    switch_count++;
    return;
  case ND_CASE:
    assert(node->lhs->kind == ND_NUM);
    printf(".Lcase%d_%d:\n", switch_count, node->lhs->val);
    gen(node->rhs);
    return;
  case ND_DEFAULT:
    printf(".Ldefault%d:\n", switch_count);
    return;
  case ND_BREAK:
    printf("  jmp .Lbreak%d\n", break_count);
    return;
  case ND_CONTINUE:
    printf("  jmp .Lcontinue%d\n", continue_count);
    return;
  case ND_WHILE:
    l = label_count++;
    printf(".Lbegin%d:\n", l);
    printf(".Lcontinue%d:\n", continue_count);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l);
    gen(node->rhs);
    printf("  jmp .Lbegin%d\n", l);
    printf(".Lend%d:\n", l);
    continue_count++;
    return;
  case ND_FOR:
    l = label_count++;
    if(node->lhs->lhs)
      gen(node->lhs->lhs);
    printf(".Lbegin%d:\n", l);
    if (node->lhs->rhs)
      gen(node->lhs->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l);
    if (node->rhs->rhs)
      gen(node->rhs->rhs);
    printf(".Lcontinue%d:\n", continue_count);
    gen(node->rhs->lhs);
    printf("  jmp .Lbegin%d\n", l);
    printf(".Lend%d:\n", l);
    printf("  pop rax\n");
    continue_count++;
    return;
  case ND_BLOCK:
    for (n = node->rhs; n; n = n->rhs)
    {
      gen(n->lhs);
      printf("  pop rax\n");
    }
    printf("  push 0\n");
    return;
  case ND_CALL:
    n = node->rhs;
    i = 0;

    while (n)
    {
      gen(n->lhs);
      n = n->rhs;
      i++;
    }
    if (i > 6)
      error_at(node->lhs->name->ptr, "too many arguments");

    char *regs[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

    for (int j = i - 1; j >= 0; j--)
      printf("  pop %s\n", regs[j]);

    printf("  mov r10, rsp\n");
    printf("  and rsp, 0xfffffffffffffff0\n");
    printf("  push r10\n");
    printf("  push 0\n");

    printf("  call %.*s\n", node->lhs->name->len, node->lhs->name->ptr);

    printf("  pop rdi\n");
    printf("  pop rdi\n");

    printf("  mov rsp, rdi\n");
    printf("  push rax\n");

    return;
  case ND_ADDR:
    gen_lval(node->lhs);
    return;
  case ND_DEREF:
    gen(node->lhs);
    printf("  pop rax\n");
    switch (sizeof_type(node->type))
    {
    case 4:
      printf("  mov eax, [rax]\n");
      break;
    case 8:
      printf("  mov rax, [rax]\n");
      break;
    case 1:
      printf("  movsx rax, BYTE PTR [rax]\n");
      break;
    default:
      error("unexpected type");
    }

    printf("  push rax\n");
    return;
  case ND_AND:
    l = label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lfalse%d\n", l);
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lfalse%d\n", l);
    printf("  push 1\n");
    printf("  jmp .Lend%d\n", l);
    printf(".Lfalse%d:\n", l);
    printf("  push 0\n");
    printf(".Lend%d:\n", l);
    return;
  case ND_OR:
    l = label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 1\n");
    printf("  je .Ltrue%d\n", l);
    gen(node->rhs);
    printf("  pop rax\n");
    printf("  cmp rax, 1\n");
    printf("  je .Ltrue%d\n", l);
    printf("  push 0\n");
    printf("  jmp .Lend%d\n", l);
    printf(".Ltrue%d:\n", l);
    printf("  push 1\n");
    printf(".Lend%d:\n", l);
    return;
  default:
    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    default:
      error("unreachable");
      return;
    }

    printf("  push rax\n");
  }
}
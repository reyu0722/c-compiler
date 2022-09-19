#ifdef __STDC__
#include <stdio.h>
#endif
#include "codegen.h"
#include "error.h"
#include "header.h"

int label_count;
int switch_count;
int max_switch_count;
int break_count;
int max_break_count;
int continue_count;
int max_continue_count;

int stack;

void gen(Node *node);

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

void gen_stmt(Node *node)
{
  Node *n;
  int l;
  switch (node->kind)
  {
  case ND_BLOCK:
    for (n = node->rhs; n; n = n->rhs)
      gen_stmt(n->lhs);
    return;
  case ND_IF:
    l = label_count;
    label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l);
    gen_stmt(node->rhs);
    printf(".Lend%d:\n", l);
    return;
  case ND_IFELSE:
    l = label_count;
    label_count++;
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", l);
    gen_stmt(node->rhs->lhs);
    printf("  jmp .Lend%d\n", l);
    printf(".Lelse%d:\n", l);
    gen_stmt(node->rhs->rhs);
    printf(".Lend%d:\n", l);
    return;
  case ND_SWITCH:
    assert(node->rhs->kind == ND_BLOCK);
    gen(node->lhs);
    printf("  pop rax\n");

    int i = break_count;
    max_break_count++;
    break_count = max_break_count;
    int b = break_count;

    int j = switch_count;
    max_switch_count++;
    switch_count = max_switch_count;

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

    gen_stmt(node->rhs);
    printf(".Lbreak%d:\n", b);
    break_count = i;
    switch_count = j;
    return;
  case ND_CASE:
    assert(node->lhs->kind == ND_NUM);
    printf(".Lcase%d_%d:\n", switch_count, node->lhs->val);
    return;
  case ND_DEFAULT:
    printf(".Ldefault%d:\n", switch_count);
    return;
  case ND_WHILE:
    l = label_count;
    label_count++;

    i = continue_count;
    max_continue_count++;
    continue_count = max_continue_count;

    printf(".Lbegin%d:\n", l);
    printf(".Lcontinue%d:\n", continue_count);
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", l);
    gen_stmt(node->rhs);
    printf("  jmp .Lbegin%d\n", l);
    printf(".Lend%d:\n", l);
    continue_count = i;
    return;
  case ND_FOR:
    l = label_count;
    label_count++;

    i = continue_count;
    max_continue_count++;
    continue_count = max_continue_count;
    j = max_continue_count;

    int k = break_count;
    max_break_count++;
    break_count = max_break_count;
    b = max_break_count;

    if (node->lhs->lhs)
      gen_stmt(node->lhs->lhs);

    printf(".Lbegin%d:\n", l);
    if (node->lhs->rhs)
    {
      gen(node->lhs->rhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", l);
    }
    gen_stmt(node->rhs->rhs);
    printf(".Lcontinue%d:\n", j);
    if (node->rhs->lhs)
      gen_stmt(node->rhs->lhs);

    printf("  jmp .Lbegin%d\n", l);
    printf(".Lend%d:\n", l);
    printf(".Lbreak%d:\n", b);
    continue_count = i;
    break_count = k;
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
  case ND_BREAK:
    printf("  jmp .Lbreak%d\n", break_count);
    return;
  case ND_CONTINUE:
    printf("  jmp .Lcontinue%d\n", continue_count);
    return;
  default:
    gen(node);
    printf("  pop rax\n");
  }
}

void gen(Node *node)
{
  int l;
  int i;
  int j;
  int k;
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
    int size = sizeof_type(node->type);
    for (;;)
    {
      if (size >= 8)
      {
        printf("  mov r10, [rax + %d]\n", sizeof_type(node->type) - size);
        printf("  push r10\n");
        size -= 8;
      }
      else if (size == 4)
      {
        printf("  movsxd rax, [rax]\n");
        printf("  push rax\n");
        size -= 4;
      }
      else if (size == 1)
      {
        printf("  movsx rax, BYTE PTR [rax]\n");
        printf("  push rax\n");
        size -= 1;
      }
      else
        error("not implemented: size %d", size);

      if (size == 0)
        break;
    }

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
      printf("  mov DWORD PTR [rax], edi\n");
      break;
    case 1:
      printf("  mov BYTE PTR [rax], dil\n");
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
  case ND_CALL:
    n = node->rhs;
    i = 0;

    while (n)
    {
      gen(n->lhs);
      if (sizeof_type(n->lhs->type) > 32)
        error("not implemented: too big object");
      i += (sizeof_type(n->lhs->type) + 7) / 8;
      n = n->rhs;
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
    printf("  mov rax, 0\n");

    printf("  call %.*s\n", node->lhs->name->len, node->lhs->name->ptr);

    printf("  pop rdi\n");
    printf("  pop rdi\n");

    printf("  mov rsp, rdi\n");

    if (node->type->ty == VOID)
    {
      printf("  push 0\n");
      return;
    }

    switch (sizeof_type(node->type))
    {
    case 1:
      printf("  movsx rax, al\n");
      printf("  push rax\n");
      break;
    case 4:
      printf("  movsx rax, eax\n");
      printf("  push rax\n");
      break;
    case 8:
      printf("  push rax\n");
      break;
    default:
      error("not implemented: return value");
    }

    return;
  case ND_VA_START:
    n = node->rhs->lhs;
    assert(n->kind == ND_LVAR);

    printf("  mov eax, %d\n", arg_count * 8);
    printf("  mov [rbp - %d], eax\n", n->offset);
    printf("  mov eax, 48\n");
    printf("  mov [rbp - %d], eax\n", n->offset - 4);
    printf("  lea rax, [rbp + 16]\n");
    printf("  mov [rbp - %d], rax\n", n->offset - 8);
    printf("  lea rax, [rbp - %d]\n", current_stack_size + 48);
    printf("  mov [rbp - %d], rax\n", n->offset - 16);

    printf("  push 0\n");
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
    l = label_count;
    label_count++;
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
    l = label_count;
    label_count++;
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
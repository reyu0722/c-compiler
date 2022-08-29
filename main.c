#include "mycc.h"

char *user_input;
char *regs[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void error(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "invalid argument");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  while (!at_eof())
  {
    External *ext = external();

    switch (ext->kind)
    {
    case FUNC:
      for (StringLiteral *l = ext->literals; l; l = l->next)
      {
        printf(".data\n");
        printf(".LC%d:\n", l->offset);
        printf("  .string \"%.*s\"\n", l->len, l->str);
      }

      printf(".text\n");
      printf("%.*s:\n", ext->len, ext->name);
      printf("  push rbp\n");
      printf("  mov rbp, rsp\n");
      printf("  sub rsp, 208\n");

      for (int i = 0; i < 6 && ext->offsets[i]; i++)
        printf("  mov [rbp - %d], %s\n", ext->offsets[i], regs[i]);

      for (int i = 0; ext->code[i]; i++)
      {
        gen(ext->code[i]);
        printf("  pop rax\n");
      }

      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      break;
    case GVAR:
      printf(".data\n");
      printf("%.*s:\n", ext->len, ext->name);
      printf("  .zero %d\n", ext->size);
      break;
    }
  }

  return 0;
}

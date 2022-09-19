#ifdef __STDC__
#include <stdio.h>
#include <stdlib.h>
#else
void *calloc();
#endif
#include "codegen.h"
#include "error.h"
#include "file.h"
#include "header.h"
#include "parse.h"
#include "preprocess.h"
#include "tokenize.h"

char *user_input;
char *filename;
char *dir_name;
int current_stack_size;
int arg_count;

int main(int argc, char **argv)
{
  char *regs1[6] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
  char *regs4[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
  char *regs8[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

  if (argc != 2)
  {
    error("invalid argument");
    return 1;
  }

  filename = argv[1];
  dir_name = get_dir(filename);

  user_input = read_file(filename);
  token = tokenize(user_input, 1);
  token = preprocess(token);

  printf(".intel_syntax noprefix\n");
  printf(".data\n");
  printf("NULL:\n");
  printf("  .zero 8\n");

  while (!at_eof())
  {
    External *ext = external();

    switch (ext->kind)
    {
    case EXT_FUNC:
      gen_string_literal(ext->literals);

      printf(".globl %.*s\n", ext->name->len, ext->name->ptr);
      printf(".text\n");
      printf("%.*s:\n", ext->name->len, ext->name->ptr);
      printf("  push rbp\n");
      printf("  mov rbp, rsp\n");
      printf("  sub rsp, %d\n", ext->stack_size);

      if (ext->is_variadic)
        for (int i = 5; i >= 0; i--)
          printf("  push %s\n", regs8[i]);

      int regi = 0;
      int i;
      for (i = 0; i < 6 && ext->offsets[i]; i++)
      {
        char *reg;
        int size;
        if (i == 0)
          size = ext->offsets[i];
        else
          size = ext->offsets[i] - ext->offsets[i - 1];

        int base = size;
        for (;;)
        {
          int s = size;

          if (size >= 8)
          {
            reg = regs8[regi];
            size -= 8;
          }
          else if (size == 4)
          {
            reg = regs4[regi];
            size -= 4;
          }
          else if (size == 1)
          {
            reg = regs1[regi];
            size -= 1;
          }
          else
            error("not implemented: size %d", size);

          printf("  mov [rbp - %d], %s\n", ext->offsets[i] + (s - base), reg);
          regi++;
          if (size == 0)
            break;
        }
      }

      arg_count = i;
      current_stack_size = ext->stack_size;

      gen_stmt(ext->code);

      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      break;
    case EXT_GVAR:
      if (!ext->is_extern)
      {
        printf(".globl %.*s\n", ext->name->len, ext->name->ptr);
        printf(".data\n");
        printf("%.*s:\n", ext->name->len, ext->name->ptr);
        printf("  .zero %d\n", ext->size);
      }
      break;
    case EXT_ENUM:
    case EXT_STRUCT:
    case EXT_FUNCDECL:
    case EXT_TYPEDEF:
      break;
    }
  }

  return 0;
}

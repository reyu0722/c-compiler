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
  token = tokenize(user_input, true);
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
      gen_function(ext);
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

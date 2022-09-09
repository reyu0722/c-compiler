#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "codegen.h"
#include "error.h"
#include "header.h"
#include "parse.h"
#include "preprocess.h"
#include "tokenize.h"

char *user_input;
char *filename;
char *dir_name;
char *regs[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};

char *read_file(char *path)
{
  FILE *fp = fopen(path, "r");
  if (!fp)
    error("cannot open %s: %s", path, strerror(errno));

  if (fseek(fp, 0, SEEK_END) == -1)
    error("%s: fseek: %s", path, strerror(errno));
  size_t size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1)
    error("%s: fseek: %s", path, strerror(errno));

  char *buf = calloc(1, size + 2);
  fread(buf, size, 1, fp);

  if (size == 0 || buf[size - 1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\0';
  fclose(fp);
  return buf;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "invalid argument");
    return 1;
  }

  filename = argv[1];

  int dir;
  for (dir = strlen(filename) - 1; dir >= 0; dir--)
  {
    if (filename[dir] == '/')
      break;
  }

  dir_name = calloc(1, dir + 1);
  strncpy(dir_name, filename, dir);

  user_input = read_file(filename);
  token = tokenize(user_input, true);

  token = preprocess(token);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  while (!at_eof())
  {
    External *ext = external();

    switch (ext->kind)
    {
    case EXT_FUNC:
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
      printf("  sub rsp, %d\n", ext->stack_size);

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
    case EXT_GVAR:
      printf(".data\n");
      printf("%.*s:\n", ext->len, ext->name);
      printf("  .zero %d\n", ext->size);
      break;
    case EXT_ENUM:
    case EXT_STRUCT:
    case EXT_FUNCDECL:
      break;
    }
  }

  return 0;
}

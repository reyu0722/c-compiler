#include "header.h"

char *user_input;
char *filename;
char *regs[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};

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

  char *line = loc;
  while (user_input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while (*end != '\n')
    end++;

  int line_num = 1;
  for (char *p = user_input; p < line; p++)
    if (*p == '\n')
      line_num++;

  int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

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

  user_input = read_file(filename);
  token = tokenize(user_input);

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
      break;
    }
  }

  return 0;
}

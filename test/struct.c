struct Test;

struct Test
{
  int a;
  int b;
  char c;
};

struct Test2
{
  struct Test *st;
  int c;
  int *d;
  struct Test2 *str;
};

int main()
{
  struct Test a;
  assert(12, sizeof(a));
  assert(32, sizeof(struct Test2));

  a.a = 1;
  a.b = 2;
  assert(1, a.a);
  assert(2, a.b);

  struct Test *b = &a;
  assert(1, b->a);
  assert(2, b->b);

  struct
  {
    int a;
    int b;
  } c;

  assert(8, sizeof(c));

  struct Test2 d;
  d.st = calloc(1, sizeof(struct Test));
  d.st->a = 12;
  d.st->b = -43;
  d.c = 11;
  int x = 123;
  d.d = &x;
  d.str = &d;

  assert(12, d.st->a);
  assert(-43, d.st->b);
  assert(11, d.c);
  assert(123, *(d.d));
  assert(11, d.str->c);

  return 0;
}
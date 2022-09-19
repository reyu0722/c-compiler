#ifdef __STDC__
#include <stdlib.h>
#else
void *calloc();
#endif

struct Test;

struct Test
{
  int a;
  int b;
  char c;
  char d;
};

struct Test2
{
  struct Test *st;
  int c;
  int *d;
  struct Test2 *str;
};

struct Test3
{
  int a[100];
  int x[6];
  struct Test2 *b[10];
  int i;
  _Bool d;
  _Bool e;
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
  assert(4, b->a + 3);
  assert(7, b->b + 5);

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

  struct Test2 *p = &d;
  assert(1, p->st != NULL);
  assert(1, p->str != NULL);
  assert(12, p->st->a);
  assert(-43, p->st->b);
  assert(11, p->c);

  struct Test *p2 = p->st;
  assert(1, p2 != NULL);
  assert(12, p2->a);

  assert(12, d.st->a);
  assert(-43, d.st->b);
  assert(11, d.c);
  assert(123, *(d.d));
  assert(11, d.str->c);

  struct Test3 e;
  assert(512, sizeof(struct Test3));
  e.a[0] = 1;
  e.a[1] = 2;
  e.a[2] = 3;

  assert(1, e.a[0]);
  assert(2, e.a[1]);
  assert(3, e.a[2]);

  struct Test3 *f = &e;
  assert(1, f != NULL);
  assert(1, f->a[0]);
  assert(2, f->a[1]);
  assert(3, f->a[2]);

  f->b[0] = calloc(1, sizeof(struct Test2));
  assert(1, f->b[0] != NULL);
  assert(0, f->b[1] != NULL);

  int i = f->a[2];
  assert(3, i);
  int j = i;
  assert(3, j);

  return 0;
}
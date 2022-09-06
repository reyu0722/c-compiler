struct Test
{
  int a;
  int b;
};

int main()
{
  struct Test a;
  assert(8, sizeof(a));

  a.a = 1;
  a.b = 2;
  assert(1, a.a);
  assert(2, a.b);

  struct Test *b = &a;
  assert(1, b->a);
  assert(2, b->b);

  return 0;
}
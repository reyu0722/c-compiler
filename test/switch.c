int main()
{
  int a = 2;
  int b;
  int c = 3;

  switch (a)
  {
  case 1:
    b = 2;
    c = 1;
  case 2:
    b = 3;
  case 3:
    b = 4;
  }

  assert(4, b);
  assert(3, c);
}
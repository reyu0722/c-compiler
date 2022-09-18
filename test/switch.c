int test(int a)
{
  int b;
  switch (a)
  {
    case 1:
      b = 2;
    case 2:
      b =  3;
      break;
    case 3:
      b = 1;
    case 4:
      b = 5;
      b = 7;
      break;
    default:
      b = -1;
    }
  return b;
}

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

  switch (a)
  {
  case 1:
    b = 2;
    break;
  case 2:
    b = 3;
    break;
  case 3:
    b = 4;
  }
  assert(3, b);

  assert(3, test(1));
  assert(3, test(2));
  assert(7, test(3));
  assert(7, test(4));
  assert(-1, test(5));
}
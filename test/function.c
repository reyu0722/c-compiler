int foo()
{
	return 10;
}

int bar(int i)
{
	return i + 3;
}

int baz(int a, int b, int c, int d, int e, int f)
{
	return a + b + c + d + e + f;
}

int fib(int i)
{
	if (i == 0)
		return 0;
	if (i == 1)
		return 1;
	return fib(i - 1) + fib(i - 2);
}

int after();

int no_args(void)
{
	return 32;
}

int chr(char c)
{
	return c;
}

int void_ptr(void *ptr)
{
	return 12;
}

int a;
void no_return()
{
	a = 10;
	return;
}

int main()
{
	assert(10, foo());
	assert(13, bar(10));
	assert(21, baz(1, 2, 3, 4, 5, 6));
	assert(55, fib(10));
	assert(20, after());
	assert(32, no_args());
	assert(97, chr('a'));

	int *p = 12;
	assert(12, void_ptr(p));
	no_return();
	assert(10, a);
}

int after()
{
	return 20;
}
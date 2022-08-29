int main()
{
	int a = 10;
	int *b = &a;
	assert(10, *b);

	*b = *b + 4;
	assert(14, *b);
	assert(14, a);
}
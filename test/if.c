int main()
{
	int a;

	if (1)
		a = 1;
	else if (0)
		a = 2;
	else
		a = 3;

	assert(1, a);
}
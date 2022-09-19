int main()
{
	int i = 1;
	assert(i, 1);
	assert(i + 1, 2);
	assert(i = 3, 3);
	assert(i, 3);

	int j = i;
	assert(1, i == j);
	i = 2;
	assert(j, 3);
	assert(i, 2);
}
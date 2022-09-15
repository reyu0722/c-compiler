int main()
{
	assert(sizeof(1), 4);
	int *p = 1;
	assert(sizeof(p), 8);
	char c = 1;
	assert(sizeof(c), 1);

	char s[3] = {1, 2, 3};
	assert(sizeof(s), 3);
	assert(sizeof(&s), 8);

	assert(sizeof(int), 4);
	assert(sizeof(char), 1);
	assert(sizeof(void *), 8);
}
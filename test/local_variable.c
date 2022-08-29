int main()
{
	int i = 1;
	assert(i, 1);
	assert(i + 1, 2);
	assert(i = 3, 3);
	assert(i, 3);
}
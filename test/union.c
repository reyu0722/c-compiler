union U
{
	int a;
	int b;
};

union V;

int main()
{
	union U u;
	assert(4, sizeof(u));
	u.a = 1;
	assert(1, u.a);

	union V
	{
		int a[2];
		int b;
	} v;

	assert(8, sizeof(v));
	return 0;
}
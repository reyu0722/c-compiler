union U {
	int a;
	int b;
};

int main()
{
	union U u;
	assert(4, sizeof(u));
	u.a = 1;
	assert(1, u.a);
	return 0;
}
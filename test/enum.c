enum a
{
	B,
	C,
	D
};

int main()
{
	assert(B, 0);
	assert(C, 1);
	assert(D, 2);

	enum E
	{
		EA,
		EB
	} e = 0;
	assert(0, EA);
	assert(1, EB);

	enum E b;
	assert(4, sizeof(b));
}
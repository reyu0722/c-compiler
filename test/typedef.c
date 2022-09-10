struct Struct
{
	int a;
	int b;
};

typedef int num;
typedef struct Struct Struct;

int main()
{
	num a = 0;
	assert(4, sizeof(a));
	Struct b;
	b.a = 1;
	assert(b.a, 1);
	return 0;
}
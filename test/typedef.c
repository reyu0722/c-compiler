struct Struct
{
	int a;
	int b;
};

typedef int num;
typedef num *ptr;
typedef struct Struct Struct;

int main()
{
	num a = 10;
	assert(4, sizeof(a));
	ptr p = &a;
	assert(8, sizeof(p));
	assert(10, *p);
	Struct b;
	b.a = 1;
	assert(b.a, 1);
	return 0;
}
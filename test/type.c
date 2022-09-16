int ga[2];
int (*gb)[2];
int (**gc)[2];
int (*gd[2])[2];
int (*x[3])();
void (*signal(int, void (*)(int)))(int);

int main()
{
	int a[2] = {1, 2};
	int(*b)[2] = &a;
	int(**c)[2] = &b;
	int(*d[2])[2] = {b, b};

	ga[0] = 1;
	ga[1] = 2;
	gb = &a;
	gc = &b;
	gd[0] = gb;
	gd[1] = gb;	

	return 0;
}
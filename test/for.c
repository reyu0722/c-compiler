int main() {
	int a;
	for (int i = 0; i < 10; i++) {
		a += i;
	}
	assert(45, a);

	int b = 0;
	for (int i = 0; i < 10; i++) {
		if (i == 5)
			continue;
		b += i;
	}
	assert(40, b);
}
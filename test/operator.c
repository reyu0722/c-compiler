int main()
{
	assert(1, 1);
	assert(6, 4 + 2);
	assert(23, 30 - 7);
	assert(27, 9 * 3);
	assert(12, 36 / 3);

	assert(26, 5 + 3 * 7);
	assert(56, (5 + 3) * 7);

	assert(1, 10 > 9);
	assert(0, 10 > 10);
	assert(0, 10 > 11);

	assert(0, 10 < 9);
	assert(0, 10 < 10);
	assert(1, 10 < 11);

	assert(1, 10 >= 9);
	assert(1, 10 >= 10);
	assert(0, 10 >= 11);

	assert(0, 10 <= 9);
	assert(1, 10 <= 10);
	assert(1, 10 <= 11);

	assert(0, 10 == 9);
	assert(1, 10 == 10);

	assert(1, 10 != 9);
	assert(0, 10 != 10);

	assert(+1, +1);
	assert(-1, -1);
	assert(-3, +(-1) + (-2));

	int a = 1;
	a++;
	assert(2, a);
	a--;
	assert(1, a);
	a += 2;
	assert(3, a);
	a -= 4;
	assert(-1, a);
	a *= -5;
	assert(5, a);
	a /= 2;
	assert(2, a);

	assert(1, !(0 == 1));
	assert(0, !(1 == 1));

	assert(1, (1 == 1) && (2 == 2));
	assert(0, (1 == 1) && (2 == 3));
	assert(0, (1 == 2) && (2 == 3));
	assert(0, (1 == 2) && (2 == 3));
	
	assert(1, (1 == 1) || (2 == 2));
	assert(1, (1 == 1) || (2 == 3));
	assert(1, (1 == 2) || (2 == 2));
	assert(0, (1 == 2) || (2 == 3));
}

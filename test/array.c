

int main()
{
	int arr[5] = {1, 2, 3, 4, 5};
	assert(arr[0], 1);
	assert(arr[1], 2);

	arr[1] = 3;
	assert(arr[1], 3);
	assert(*(arr + 1), 3);

	char *a[5] = {"aaa", "bb", "c", "ddd", "e"};
	assert(0, strcmp(a[0], "aaa"));
	assert(0, strcmp(a[1], "bb"));
	assert(0, strcmp(a[2], "c"));
	assert(0, strcmp(a[3], "ddd"));
	assert(0, strcmp(a[4], "e"));
}
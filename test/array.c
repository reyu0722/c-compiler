int main()
{
	int arr[5] = {1, 2, 3, 4, 5};
	assert(arr[0], 1);
	assert(arr[1], 2);

	arr[1] = 3;
	assert(arr[1], 3);
	assert(*(arr + 1), 3);
}
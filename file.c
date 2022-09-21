#ifdef __STDC__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
typedef void FILE;
typedef int size_t;
void *calloc();
FILE *fopen();
#endif
#include "error.h"

char *read_file(char *path)
{
	FILE *fp = fopen(path, "r");
	if (!fp)
		error("cannot open %s", path /*, strerror(errno) */);

	if (fseek(fp, 0, /*SEEK_END*/ 2) == -1)
		error("%s: fseek", path /*, strerror(errno) */);
	size_t size = ftell(fp);
	if (fseek(fp, 0, /*SEEK_SET*/ 0) == -1)
		error("%s: fseek", path /*, strerror(errno)*/);

	char *buf = calloc(1, size + 2);
	fread(buf, size, 1, fp);

	if (size == 0 || buf[size - 1] != '\n')
		buf[size++] = '\n';

	buf[size] = '\0';
	fclose(fp);
	return buf;
}

char *get_dir(char *path)
{
	int i;
	for (i = strlen(path) - 1; i >= 0; i--)
	{
		if (path[i] == '/')
			break;
	}

	char *dir;
	if (i == -1)
	{
		dir = calloc(1, 2);
		strncpy(dir, ".", 1);
		return dir;
	}
	dir = calloc(1, i + 1);
	strncpy(dir, path, i);

	return dir;
}

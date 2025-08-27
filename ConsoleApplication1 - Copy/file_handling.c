#include "tinydir.h"
#include <stdio.h>
#include <assert.h>
#include "file_handling.h"


FileNames foo() {
	tinydir_dir dir;
	const char* str = "C:\\Users\\Elior\\source\\repos\\Project1\\Debug\\Resources\\Textures";
	int err = tinydir_open(&dir, str);
	assert(err != -1);

	FileNames names = {};

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		printf("%s", file.name);
		if (file.is_dir)
		{
			printf("/");
		}
		printf("\n");

		tinydir_next(&dir);

		if (!file.is_dir && strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
		{
			strcpy(names.names[names.num], file.name);
			names.num++;
		}
	}

	tinydir_close(&dir);

	return names;
}
#include <stdio.h>
#include <stdlib.h>

#include "lex.h"

char* read_file(char* path)
{
	FILE* f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* contents = malloc(fsize + 1);
	fread(contents, 1, fsize, f);
	fclose(f);

	contents[fsize] = 0;

	return contents;
}

int main(int argc, char** argv)
{
	if(argc == 2)
	{
		char* source= read_file(argv[1]);
		token_t* tokens = lex(source);

		for(int i = 0; i < sb_count(tokens); i++)
		{
			printf("%s\n", token_type_names[tokens[i].type]);
		}
	}
	else
	{
		printf("usage: %s [file]\n", argv[0]);
	}

	return 0;
}

#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "parser.h"

#include "ast_printer.h"

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
		program_t* program = parse(tokens);

		FILE* f = fopen("out.c", "wb");
		print_ast(f, program);
		fclose(f);
	}
	else
	{
		printf("usage: %s [file]\n", argv[0]);
	}

	return 0;
}

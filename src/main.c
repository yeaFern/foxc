#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "parser.h"

#include "ast_printer.h"
#include "generator.h"

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
	if(argc == 1)
	{
		while(true)
		{
			printf("> ");

			char source[256];
			gets(source);
			token_t* tokens = lex(source);
			expr_t* expression = parse_expression(tokens);

			print_expression(stdout, expression);
		}
	}
	if(argc == 2)
	{
		char* source = read_file(argv[1]);
		token_t* tokens = lex(source);
		program_t* program = parse(tokens);

		print_ast(stdout, program);

		FILE* f = fopen("out.s", "wb");
		generate(f, program);
		fclose(f);

		free(source);
	}
	else
	{
		printf("usage: %s [file]\n", argv[0]);
	}

	return 0;
}

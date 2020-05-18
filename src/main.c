#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "parser.h"

#include "ast_printer.h"
#include "generator.h"

char* read_file(char* path)
{
	FILE* f = fopen(path, "rb");

	if(f == NULL)
	{
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* contents = malloc(fsize + 1);
	fread(contents, 1, fsize, f);
	fclose(f);

	contents[fsize] = 0;

	return contents;
}

#define C_RESET     "\033[0m"
#define C_GRAY      "\033[30;1m"
#define C_RED       "\033[0;31m"
#define C_GREEN     "\033[0;32m"
#define C_YELLOW    "\033[0;33m"
#define C_BLUE      "\033[0;34m"
#define C_MAGENTA   "\033[0;35m"
#define C_CYAN      "\033[0;36m"
#define C_WHITE     "\033[37;0m"

#define C_BOLD_RED   "\033[1;31m"
#define C_BOLD_GREEN "\033[1;32m"
#define C_BOLD_BLUE  "\033[1;34m"

void print_token(token_t* t)
{
	int line = 0;
    printf(C_CYAN "%4d" C_RESET " %-12s " C_RED, line, token_type_names[t->type]);
    switch(t->type)
    {
    case TKN_INTEGER: { printf("%ld", t->val_integer); break; }
    case TKN_IDENT:   { printf("%s",  t->val_string ); break; }
    default: break;
    }
    printf(C_RESET "\n");
}

void print_tokens(token_t* tokens)
{
    printf(C_BOLD_GREEN "line type         value" C_RESET "\n");
    for(int i = 0; i < sb_count(tokens); i++)
    {
        print_token(&tokens[i]);
    }
}

typedef struct
{
    char* buffer;
    size_t buffer_length;
    int input_length;
} input_buf_t;

input_buf_t read_input()
{
    input_buf_t buf;
    buf.buffer = 0;

    int r = getline(&(buf.buffer), &(buf.buffer_length), stdin);

    buf.input_length = r - 1;
    buf.buffer[r - 1] = 0;

    return buf;
}

void run_repl()
{
    for(;;)
    {
        printf("> ");
        input_buf_t b = read_input();

        token_t* tokens = lex(b.buffer);
        print_tokens(tokens);
        free(b.buffer);

		stmt_t* statement = _parse_statement(tokens);

		print_statement(stdout, statement);
    }
}

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		run_repl();
	}
	if(argc == 2)
	{
		char* source = read_file(argv[1]);

		if(source == NULL)
		{
			printf("unable to open file '%s'\n", argv[1]);
			return 1;
		}

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

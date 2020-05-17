#include "ast_printer.h"

static void print_expr(expr_t* expr)
{
	switch(expr->type)
	{
	case EXPR_LITERAL: {
		printf("%ld", expr->value);
	} break;
	}
}

static void print_stmt(stmt_t* stmt)
{
	switch(stmt->type)
	{
	case STMT_RETURN: {
		printf("return ");
		print_expr(stmt->expr);
		printf(";\n");
	} break;
	}
}

static void print_decl(decl_t* decl)
{
	switch(decl->type)
	{
	case DECL_FUNC: {
		printf("int %s () {\n", decl->name);
		print_stmt(decl->stmt);
		printf("}\n");
	} break;
	}
}

static void print_program(program_t* program)
{
	print_decl(program->decl);
}

void print_ast(program_t* program)
{
    print_program(program);
}

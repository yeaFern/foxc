#include "ast_printer.h"

static struct
{
	FILE* handle;
} state;

static void print_expr(expr_t* expr)
{
	switch(expr->type)
	{
	case EXPR_LITERAL: {
		fprintf(state.handle, "%ld", expr->value);
	} break;
	case EXPR_UNARY: {
		switch(expr->unary_operator)
		{
		case UNARY_NEGATE:             { fprintf(state.handle, "-"); break; }
		case UNARY_BITWISE_COMPLEMENT: { fprintf(state.handle, "~"); break; }
		case UNARY_LOGICAL_NEGATE:     { fprintf(state.handle, "!"); break; }
		default: UNHANDLED_CASE();
		};

		print_expr(expr->unary_operand);
	} break;
	default: {
		UNHANDLED_CASE();
	} break;
	}
}

static void print_stmt(stmt_t* stmt)
{
	switch(stmt->type)
	{
	case STMT_RETURN: {
		fprintf(state.handle, "return ");
		print_expr(stmt->expr);
		fprintf(state.handle, ";\n");
	} break;
	default: {
		UNHANDLED_CASE();
	} break;
	}
}

static void print_decl(decl_t* decl)
{
	switch(decl->type)
	{
	case DECL_FUNC: {
		fprintf(state.handle, "int %s () {\n", decl->name);
		print_stmt(decl->stmt);
		fprintf(state.handle, "}\n");
	} break;
	default: {
		UNHANDLED_CASE();
	} break;
	}
}

static void print_program(program_t* program)
{
	print_decl(program->decl);
}

void print_ast(FILE* handle, program_t* program)
{
	state.handle = handle;

	print_program(program);
}

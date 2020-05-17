#include "generator.h"

static struct
{
	FILE* handle;
} state;

static void generate_expr(expr_t* expr)
{
	switch(expr->type)
	{
	case EXPR_LITERAL: {
		fprintf(state.handle, "\tmovl $%ld, %%eax\n", expr->value);
	} break;
	}
}

static void generate_stmt(stmt_t* stmt)
{
	switch(stmt->type)
	{
	case STMT_RETURN: {
		generate_expr(stmt->expr);
		fprintf(state.handle, "\tret\n");
	} break;
	}
}

static void generate_decl(decl_t* decl)
{
	switch(decl->type)
	{
	case DECL_FUNC: {
		fprintf(state.handle, ".globl _%s\n", decl->name);
		fprintf(state.handle, "_%s:\n", decl->name);
		generate_stmt(decl->stmt);
	} break;
	}
}

static void generate_program(program_t* program)
{
	generate_decl(program->decl);
}

void generate(FILE* handle, program_t* program)
{
	state.handle = handle;

	generate_program(program);
}
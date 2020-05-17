#include "generator.h"

static struct
{
	FILE* handle;
} state;

static void generate_expr(expr_t* expr);

static void generate_unary_expr(expr_t* expr)
{
	switch(expr->unary_operator)
	{
	case UNARY_NEGATE: {
		generate_expr(expr->unary_operand);
		fprintf(state.handle, "\tneg %%eax\n");
	} break;
	case UNARY_BITWISE_COMPLEMENT: {
		generate_expr(expr->unary_operand);
		fprintf(state.handle, "\tnot %%eax\n");
	} break;
	case UNARY_LOGICAL_NEGATE: {
		generate_expr(expr->unary_operand);
		fprintf(state.handle, "\tcmpl $0, %%eax\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsete %%al\n");
	} break;
	default: {
		UNHANDLED_CASE();
	} break;
	}
}

static void generate_binary_expr(expr_t* expr)
{
	switch(expr->binary_operator)
	{
	case BINARY_ADD: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\taddl %%ecx, %%eax\n");
	} break;
	case BINARY_SUB: {
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tsubl %%ecx, %%eax\n");
	} break;
	case BINARY_MUL: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\timul %%ecx, %%eax\n");
	} break;
	case BINARY_DIV: {
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tmovl %%eax, %%ebx\n");
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\txor %%edx, %%edx\n");
		fprintf(state.handle, "\tidivl %%ebx\n");
	} break;
	default: {
		UNHANDLED_CASE();
	} break;
	}
}

static void generate_expr(expr_t* expr)
{
	switch(expr->type)
	{
	case EXPR_LITERAL: {
		fprintf(state.handle, "\tmovl $%ld, %%eax\n", expr->value);
	} break;
	case EXPR_UNARY: {
		generate_unary_expr(expr);
	} break;
	case EXPR_BINARY: {
		generate_binary_expr(expr);
	} break;
	default: {
		UNHANDLED_CASE();
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
	default: {
		UNHANDLED_CASE();
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
	default: {
		UNHANDLED_CASE();
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
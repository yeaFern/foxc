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
	case EXPR_BINARY: {
		fprintf(state.handle, "(");
		print_expr(expr->binary_lhs);
		fprintf(state.handle, " ");

		switch(expr->binary_operator)
		{
		case BINARY_ADD:         { fprintf(state.handle, "+" ); break; }
		case BINARY_SUB:         { fprintf(state.handle, "-" ); break; }
		case BINARY_MUL:         { fprintf(state.handle, "*" ); break; }
		case BINARY_DIV:         { fprintf(state.handle, "/" ); break; }
		case BINARY_LESS:        { fprintf(state.handle, "<" ); break; }
		case BINARY_LESS_EQ:     { fprintf(state.handle, "<="); break; }
		case BINARY_GRTR:        { fprintf(state.handle, ">" ); break; }
		case BINARY_GRTR_EQ:     { fprintf(state.handle, ">="); break; }
		case BINARY_EQUALS:      { fprintf(state.handle, "=="); break; }
		case BINARY_NOT_EQ:      { fprintf(state.handle, "!="); break; }
		case BINARY_LOGICAL_AND: { fprintf(state.handle, "&&"); break; }
		case BINARY_LOGICAL_OR:  { fprintf(state.handle, "||"); break; }
		case BINARY_MODULO:      { fprintf(state.handle, "%%"); break; }
		case BINARY_BITWISE_AND: { fprintf(state.handle, "&" ); break; }
		case BINARY_BITWISE_OR:  { fprintf(state.handle, "|" ); break; }
		case BINARY_BITWISE_XOR: { fprintf(state.handle, "^" ); break; }
		case BINARY_SHIFT_LEFT:  { fprintf(state.handle, "<<"); break; }
		case BINARY_SHIFT_RIGHT: { fprintf(state.handle, ">>"); break; }
		default: UNHANDLED_CASE();
		}

		fprintf(state.handle, " ");
		print_expr(expr->binary_rhs);
		fprintf(state.handle, ")");
	} break;
	case EXPR_ASSIGNMENT: {
		fprintf(state.handle, "(%s = ", expr->assign_name);
		print_expr(expr->assign_rhs);
		fprintf(state.handle, ")");
	} break;
	case EXPR_VAR: {
		fprintf(state.handle, "%s", expr->var_name);
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
	case STMT_EXPR: {
		print_expr(stmt->standalone_expr);
		fprintf(state.handle, ";\n");
	} break;
	case STMT_RETURN: {
		fprintf(state.handle, "return ");
		print_expr(stmt->return_expr);
		fprintf(state.handle, ";\n");
	} break;
	case STMT_DECLARE: {
		fprintf(state.handle, "int %s", stmt->declare_name);
		if(stmt->declare_initializer)
		{
			fprintf(state.handle, " = ");
			print_expr(stmt->declare_initializer);
		}
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
		for(int i = 0; i < sb_count(decl->stmts); i++)
		{
			print_stmt(decl->stmts[i]);
		}
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
	fprintf(state.handle, "\n");
}

void print_expression(FILE* handle, expr_t* expression)
{
	state.handle = handle;

	print_expr(expression);
	fprintf(state.handle, "\n");
}

void print_statement(FILE* handle, stmt_t* statement)
{
	state.handle = handle;

	print_stmt(statement);
	fprintf(state.handle, "\n");
}
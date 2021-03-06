#include "generator.h"

typedef struct
{
	str_t name;
	int stack_offset;
} var_map_entry_t;

static struct
{
	FILE* handle;
	int label_counter;

	var_map_entry_t* var_map;
	int stack_index;
} state;

static char* new_label()
{
	char* buffer = malloc(64);
	snprintf(buffer, 63, "_label%d", state.label_counter);
	state.label_counter++;
	return buffer;
}

static void free_label(char* label)
{
	free(label);
}

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
	case BINARY_LESS: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tcmpl %%eax, %%ecx\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsetl %%al\n");
	} break;
	case BINARY_LESS_EQ: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tcmpl %%eax, %%ecx\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsetle %%al\n");
	} break;
	case BINARY_GRTR: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tcmpl %%eax, %%ecx\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsetg %%al\n");
	} break;
	case BINARY_GRTR_EQ: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tcmpl %%eax, %%ecx\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsetge %%al\n");
	} break;
	case BINARY_DIV: {
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tmovl %%eax, %%ebx\n");
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\txor %%edx, %%edx\n");
		fprintf(state.handle, "\tidivl %%ebx\n");
	} break;
	case BINARY_EQUALS: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tcmpl %%eax, %%ecx\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsete %%al\n");
	} break;
	case BINARY_NOT_EQ: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tcmpl %%eax, %%ecx\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsetne %%al\n");
	} break;
	case BINARY_LOGICAL_AND: {
		char* l1 = new_label();
		char* l2 = new_label();

		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tcmpl $0, %%eax\n");
		fprintf(state.handle, "\tjne %s\n", l1);
		fprintf(state.handle, "\tjmp %s\n", l2);
		fprintf(state.handle, "%s:\n", l1);
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tcmpl $0, %%eax\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsetne %%al\n");
		fprintf(state.handle, "%s:\n", l2);

		free_label(l1);
		free_label(l2);
	} break;
	case BINARY_LOGICAL_OR: {
		char* l1 = new_label();
		char* l2 = new_label();

		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tcmpl $0, %%eax\n");
		fprintf(state.handle, "\tje %s\n", l1);
		fprintf(state.handle, "\tmovl $1, %%eax\n");
		fprintf(state.handle, "\tjmp %s\n", l2);
		fprintf(state.handle, "%s:\n", l1);
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tcmpl $0, %%eax\n");
		fprintf(state.handle, "\tmovl $0, %%eax\n");
		fprintf(state.handle, "\tsetne %%al\n");
		fprintf(state.handle, "%s:\n", l2);

		free_label(l1);
		free_label(l2);
	} break;
	case BINARY_MODULO: {
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tmovl %%eax, %%ebx\n");
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\txor %%edx, %%edx\n");
		fprintf(state.handle, "\tidivl %%ebx\n");
		fprintf(state.handle, "\tmovl %%edx, %%eax\n");
	} break;
	case BINARY_BITWISE_AND: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tand %%ecx, %%eax\n");
	} break;
	case BINARY_BITWISE_OR: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tor %%ecx, %%eax\n");
	} break;
	case BINARY_BITWISE_XOR: {
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\txor %%ecx, %%eax\n");
	} break;
	case BINARY_SHIFT_LEFT: {
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tsal %%cl, %%eax\n");
	} break;
	case BINARY_SHIFT_RIGHT: {
		generate_expr(expr->binary_rhs);
		fprintf(state.handle, "\tpush %%rax\n");
		generate_expr(expr->binary_lhs);
		fprintf(state.handle, "\tpop %%rcx\n");
		fprintf(state.handle, "\tsar %%cl, %%eax\n");
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
	case EXPR_VAR: {
		int offset = 0;
		for(int i = 0; i < sb_count(state.var_map); i++)
		{
			if(state.var_map[i].name == expr->var_name)
			{
				offset = state.var_map[i].stack_offset;
				break;
			}
		}

		fprintf(state.handle, "\tmovl %d(%%rbp), %%eax\n", offset);
	} break;
	case EXPR_ASSIGNMENT: {
		generate_expr(expr->assign_rhs);

		int offset = 0;
		for(int i = 0; i < sb_count(state.var_map); i++)
		{
			if(state.var_map[i].name == expr->var_name)
			{
				offset = state.var_map[i].stack_offset;
				break;
			}
		}

		fprintf(state.handle, "\tmovl %%eax, %d(%%rbp)\n", offset);
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
		generate_expr(stmt->return_expr);

		// Function Epilogue
		fprintf(state.handle, "\tmov %%rbp, %%rsp\n");
		fprintf(state.handle, "\tpop %%rbp\n");
		fprintf(state.handle, "\tret\n");
	} break;
	case STMT_EXPR: {
		generate_expr(stmt->standalone_expr);
	} break;
	case STMT_DECLARE: {
		// if(var_exists(stmt->var_name)) ...
		if(stmt->declare_initializer)
		{
			generate_expr(stmt->declare_initializer);
		}
		fprintf(state.handle, "\tpush %%rax # %s\n", stmt->declare_name);

		state.stack_index -= 8; // TODO: calculate size of pushed value automatically

		var_map_entry_t new_var_entry;
		new_var_entry.name = stmt->declare_name;
		new_var_entry.stack_offset = state.stack_index;

		sb_push(state.var_map, new_var_entry);
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
		fprintf(state.handle, ".globl %s\n", decl->name);
		fprintf(state.handle, "%s:\n", decl->name);
		
		// Function Prologue
		fprintf(state.handle, "\tpush %%rbp\n");
		fprintf(state.handle, "\tmov %%rsp, %%rbp\n");
		
		for(int i = 0; i < sb_count(decl->stmts); i++)
		{
			generate_stmt(decl->stmts[i]);
		}
		
		// Function Epilogue
		fprintf(state.handle, "\tmov %%rbp, %%rsp\n");
		fprintf(state.handle, "\tpop %%rbp\n");
		fprintf(state.handle, "\tret\n");
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
	state.label_counter = 0;
	state.var_map = NULL;
	state.stack_index = 0;

	generate_program(program);
}
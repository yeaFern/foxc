#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>

#include "token.h"
#include "buf.h"
#include "str.h"
#include "error.h"

typedef enum
{
	EXPR_LITERAL,
	EXPR_UNARY,
	EXPR_BINARY
} expr_type_t;

typedef enum
{
	UNARY_UNKNOWN,
	UNARY_NEGATE,
	UNARY_BITWISE_COMPLEMENT,
	UNARY_LOGICAL_NEGATE
} unary_operator_t;

typedef enum
{
	BINARY_UNKNOWN,
	BINARY_ADD,
	BINARY_SUB,
	BINARY_MUL,
	BINARY_DIV
} binary_operator_t;

typedef struct expr_t
{
	expr_type_t type;

	union
	{
		struct
		{ // EXPR_LITERAL
			uint64_t value;
		};
		struct
		{ // EXPR_UNARY
			unary_operator_t unary_operator;
			struct expr_t* unary_operand;
		};
		struct
		{ // EXPR_BINARY
			binary_operator_t binary_operator;
			struct expr_t* binary_lhs;
			struct expr_t* binary_rhs;
		};
	};
} expr_t;

typedef enum
{
	STMT_RETURN
} stmt_type_t;

typedef struct
{
	stmt_type_t type;

	union
	{
		struct
		{ // STMT_RETURN
			expr_t* expr;
		};
	};
} stmt_t;

typedef enum
{
	DECL_FUNC
} decl_type_t;

typedef struct
{
	decl_type_t type;

	union
	{
		struct
		{ // DECL_FUNC
			str_t name;
			stmt_t* stmt;
		};
	};
} decl_t;

typedef struct
{
	decl_t* decl;
} program_t;

// Parses the given input, returning the root of the AST.
// If the parser encounters an error, the program will terminate and an
// error message will be printed to the user.
program_t* parse(token_t* tokens);

// Parses a single expression from given token list.
expr_t* parse_expression(token_t* tokens);

#endif

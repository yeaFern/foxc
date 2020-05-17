#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>

#include "token.h"
#include "buf.h"
#include "str.h"
#include "error.h"

typedef enum
{
	EXPR_LITERAL
} expr_type_t;

typedef struct
{
	expr_type_t type;

	union
	{
		struct
		{ // EXPR_LITERAL
			uint64_t value;
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

#endif

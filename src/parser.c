#include "parser.h"

// Global state for the parser.
// The state is reset with each call to 'parse()'.
static struct
{
	token_t* tokens;
	int ptr;
} state;

//
// State modifiers.
//

// Returns true if there are more tokens to be read from the input, false otherwise.
static bool has_next()
{
	return state.tokens[state.ptr].type != TKN_EOF;
}

// Returns the next token in the input stream, does not increment the pointer.
static token_t peek()
{
	return state.tokens[state.ptr];
}

// Returns the next token in the input stream, incremenets the internal pointer
// to point to the next token.
static token_t next()
{
	return state.tokens[state.ptr++];
}

// Returns the next token in the stream if that token matches the specified
// type, throws an error if the types do not match.
static token_t expect(token_type_t type)
{
	token_t token = next();
	if(token.type == type)
	{
		return token;
	}

	error("unexpected token '%s', expected '%s'\n", token_type_names[token.type], token_type_names[type]);
}

//
// Helper functions.
//

// Allocates a new expression with the given type.
static expr_t* new_expr(expr_type_t type)
{
	expr_t* expr = calloc(1, sizeof(expr_t));
	expr->type = type;
	return expr;
}

// Allocates a new statement with the given type.
static stmt_t* new_stmt(stmt_type_t type)
{
	stmt_t* stmt = calloc(1, sizeof(stmt_t));
	stmt->type = type;
	return stmt;
}

// Allocates a new declaration with the given type.
static decl_t* new_decl(decl_type_t type)
{
	decl_t* decl = calloc(1, sizeof(decl_t));
	decl->type = type;
	return decl;
}

// Allocates a new program.
static program_t* new_program()
{
	program_t* program = calloc(1, sizeof(program_t));
	return program;
}

//
// Parser body.
//

// Parses an expression from the input stream.
// expr = integer
static expr_t* parse_expression()
{
	token_t t = expect(TKN_INTEGER);

	// For now, the only type of expression we support is an integer literal.
	// In the future we will support binary/unary expressions too.
	expr_t* expr = new_expr(EXPR_LITERAL);
	expr->value = t.val_integer;

	return expr;
}

// Parses a statement from the input stream.
// stmt = "return" <expr> ";"
static stmt_t* parse_statement()
{
	expect(TKN_RETURN);
	expr_t* expr = parse_expression();
	expect(TKN_SEMICOLON);

	// For now, the only type of statement we support is return statements.
	// In the future we will support more statements.
	stmt_t* stmt = new_stmt(STMT_RETURN);
	stmt->expr = expr;
	return stmt;
}

// Parses a declaration from the input stream.
// decl = "int" identifier "(" ")" "{" <stmt> "}"
static decl_t* parse_declaration()
{
	token_t ret = expect(TKN_IDENT);
	// only support int types
	if(ret.val_string != _("int"))
	{
		error("'%s' does not name a type\n", ret.val_string);
	}

	token_t name = expect(TKN_IDENT);
	expect(TKN_L_PAREN);
	expect(TKN_R_PAREN);
	expect(TKN_L_CURLY);

	stmt_t* stmt = parse_statement();

	expect(TKN_R_CURLY);

	// For now we only support function delcarations.
	// In the future we will support other types of declarations.
	decl_t* decl = new_decl(DECL_FUNC);
	decl->name = name.val_string;
	decl->stmt = stmt;

	return decl;
}

// Parses a program from the input stream.
// program = <decl>
static program_t* parse_program()
{
	decl_t* decl = parse_declaration();

	program_t* program = new_program();
	program->decl = decl;
	return program;
}

//
// Public API
//

program_t* parse(token_t* tokens)
{
	state.tokens = tokens;
	state.ptr = 0;

	return parse_program();
}

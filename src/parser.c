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

// Returns the next token in the input stream, increments the internal pointer
// to point to the next token.
static token_t next()
{
	return state.tokens[state.ptr++];
}

// Returns true if the next token in the stream is of the specified type, false
// otherwise. Does not increment the internal pointer.
static bool match(token_type_t type)
{
	return peek().type == type;
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

static expr_t* parse_expr0();
static expr_t* parse_expr1();
static expr_t* parse_expr2();
static expr_t* parse_expr3();
static expr_t* parse_expr4();
static expr_t* parse_expr5();
static expr_t* parse_expr6();

// Parses an expression from the input stream.
// <"!" | "~" | "-"> <expr> | "(" <expr> ")" | integer
static expr_t* parse_expr0()
{
	if(match(TKN_INTEGER))
	{
		// If we got an integer, return a constant node.
		token_t t = next();

		expr_t* expr = new_expr(EXPR_LITERAL);
		expr->value = t.val_integer;
		return expr;
	}
	else if(match(TKN_MINUS)
		 || match(TKN_TILDE)
		 || match(TKN_BANG))
	{
		// If we got a unary operator, parse it.
		token_t t = next();

		// Deduce the operator from the token.
		unary_operator_t operator = UNARY_UNKNOWN;
		if(t.type == TKN_MINUS) { operator = UNARY_NEGATE;             }
		if(t.type == TKN_TILDE) { operator = UNARY_BITWISE_COMPLEMENT; }
		if(t.type == TKN_BANG ) { operator = UNARY_LOGICAL_NEGATE;     }

		if(operator == UNARY_UNKNOWN) { UNHANDLED_CASE(); }

		// Recursively parse the operand.
		expr_t* operand = parse_expr0();

		// Construct the unary expression.
		expr_t* expr = new_expr(EXPR_UNARY);
		expr->unary_operator = operator;
		expr->unary_operand = operand;
		return expr;
	}
	else if(match(TKN_L_PAREN))
	{
		// Recurse back to the bottom, parsing an new expression from scratch.
		expect(TKN_L_PAREN);
		expr_t* expr = parse_expr6();
		expect(TKN_R_PAREN);
		return expr;
	}
	else
	{
		error("expected an expression\n");
	}
}

// expr1 = <expr0> { ("*" | "/") <expr0> }
static expr_t* parse_expr1()
{
	expr_t* lhs = parse_expr0();
	while(match(TKN_ASTERIX)
	   || match(TKN_SLASH))
	{
		token_t t = next();
		expr_t* rhs = parse_expr0();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_ASTERIX) { operator = BINARY_MUL; }
		if(t.type == TKN_SLASH  ) { operator = BINARY_DIV; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr2 = <expr1> { ("+" | "-") <expr1> }
static expr_t* parse_expr2()
{
	expr_t* lhs = parse_expr1();
	while(match(TKN_PLUS)
	   || match(TKN_MINUS))
	{
		token_t t = next();
		expr_t* rhs = parse_expr1();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_PLUS ) { operator = BINARY_ADD; }
		if(t.type == TKN_MINUS) { operator = BINARY_SUB; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr3 = <expr2> { ("<" | "<=" | ">" | ">=") <expr2> }
static expr_t* parse_expr3()
{
	expr_t* lhs = parse_expr2();
	while(match(TKN_GT)
	   || match(TKN_GT_EQ)
	   || match(TKN_LT)
	   || match(TKN_LT_EQ))
	{
		token_t t = next();
		expr_t* rhs = parse_expr2();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_GT   ) { operator = BINARY_GRTR;    }
		if(t.type == TKN_GT_EQ) { operator = BINARY_GRTR_EQ; }
		if(t.type == TKN_LT   ) { operator = BINARY_LESS;    }
		if(t.type == TKN_LT_EQ) { operator = BINARY_LESS_EQ; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr4 = <expr3> { ("!=" | "==") <expr3> }
static expr_t* parse_expr4()
{
	// != ==
	expr_t* lhs = parse_expr3();
	while(match(TKN_EQ_EQ)
	   || match(TKN_NOT_EQ))
	{
		token_t t = next();
		expr_t* rhs = parse_expr3();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_EQ_EQ ) { operator = BINARY_EQUALS; }
		if(t.type == TKN_NOT_EQ) { operator = BINARY_NOT_EQ; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr5 = <expr4> { "&&" <expr4> }
static expr_t* parse_expr5()
{
	expr_t* lhs = parse_expr4();
	while(match(TKN_AND))
	{
		token_t t = next();
		expr_t* rhs = parse_expr4();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_AND) { operator = BINARY_LOGICAL_AND; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr6 = <expr5> { "||" <expr5> }
static expr_t* parse_expr6()
{
	expr_t* lhs = parse_expr5();
	while(match(TKN_OR))
	{
		token_t t = next();
		expr_t* rhs = parse_expr5();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_OR) { operator = BINARY_LOGICAL_OR; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// Parses a statement from the input stream.
// stmt = "return" <expr> ";"
static stmt_t* parse_statement()
{
	expect(TKN_RETURN);
	expr_t* expr = parse_expr6();
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

expr_t* parse_expression(token_t* tokens)
{
	state.tokens = tokens;
	state.ptr = 0;

	return parse_expr6();
}
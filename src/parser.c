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

static void prev()
{
	state.ptr--;
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
	decl->stmts = NULL;
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
static expr_t* parse_expr7();
static expr_t* parse_expr8();
static expr_t* parse_expr9();
static expr_t* parse_expr10();
static expr_t* parse_expr11();

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
		expr_t* expr = parse_expr11();
		expect(TKN_R_PAREN);
		return expr;
	}
	else if(match(TKN_IDENT))
	{
		token_t t = expect(TKN_IDENT);
		expr_t* expr = new_expr(EXPR_VAR);
		expr->var_name = t.val_string;
		return expr;
	}
	else
	{
		error("expected an expression\n");
	}
}

// expr1 = <expr0> { ("*" | "/" | "%") <expr0> }
static expr_t* parse_expr1()
{
	expr_t* lhs = parse_expr0();
	while(match(TKN_ASTERIX)
	   || match(TKN_SLASH)
	   || match(TKN_MODULO))
	{
		token_t t = next();
		expr_t* rhs = parse_expr0();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_ASTERIX) { operator = BINARY_MUL;    }
		if(t.type == TKN_SLASH  ) { operator = BINARY_DIV;    }
		if(t.type == TKN_MODULO ) { operator = BINARY_MODULO; }

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

// expr3 = <expr2> { "<<" ">>" <expr2> }
static expr_t* parse_expr3()
{
	expr_t* lhs = parse_expr2();
	while(match(TKN_LT_LT)
	   || match(TKN_GT_GT))
	{
		token_t t = next();
		expr_t* rhs = parse_expr2();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_LT_LT) { operator = BINARY_SHIFT_LEFT;  }
		if(t.type == TKN_GT_GT) { operator = BINARY_SHIFT_RIGHT; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr4 = <expr3> { ("<" | "<=" | ">" | ">=") <expr3> }
static expr_t* parse_expr4()
{
	expr_t* lhs = parse_expr3();
	while(match(TKN_GT)
	   || match(TKN_GT_EQ)
	   || match(TKN_LT)
	   || match(TKN_LT_EQ))
	{
		token_t t = next();
		expr_t* rhs = parse_expr3();

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

// expr5 = <expr4> { ("!=" | "==") <expr4> }
static expr_t* parse_expr5()
{
	// != ==
	expr_t* lhs = parse_expr4();
	while(match(TKN_EQ_EQ)
	   || match(TKN_NOT_EQ))
	{
		token_t t = next();
		expr_t* rhs = parse_expr4();

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

// expr6 = <expr5> { "&" <expr5> }
static expr_t* parse_expr6()
{
	expr_t* lhs = parse_expr5();
	while(match(TKN_AMP))
	{
		token_t t = next();
		expr_t* rhs = parse_expr5();

		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_AMP) { operator = BINARY_BITWISE_AND; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr7 = <expr6> { "^" <expr6> }
static expr_t* parse_expr7()
{
	expr_t* lhs = parse_expr6();
	while(match(TKN_CARET))
	{
		token_t t = next();
		expr_t* rhs = parse_expr6();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_CARET) { operator = BINARY_BITWISE_XOR; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr8 = <expr7> { "|" <expr7> }
static expr_t* parse_expr8()
{
	expr_t* lhs = parse_expr7();
	while(match(TKN_PIPE))
	{
		token_t t = next();
		expr_t* rhs = parse_expr7();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_PIPE) { operator = BINARY_BITWISE_OR; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr9 = <expr8> { "&&" <expr8> }
static expr_t* parse_expr9()
{
	expr_t* lhs = parse_expr8();
	while(match(TKN_AMP_AMP))
	{
		token_t t = next();
		expr_t* rhs = parse_expr8();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_AMP_AMP) { operator = BINARY_LOGICAL_AND; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr10 = <expr9> { "||" <expr9> }
static expr_t* parse_expr10()
{
	expr_t* lhs = parse_expr9();
	while(match(TKN_PIPE_PIPE))
	{
		token_t t = next();
		expr_t* rhs = parse_expr9();

		// Deduce the operator from the token.
		binary_operator_t operator = BINARY_UNKNOWN;
		if(t.type == TKN_PIPE_PIPE) { operator = BINARY_LOGICAL_OR; }

		if(operator == BINARY_UNKNOWN) { UNHANDLED_CASE(); }

		expr_t* expr = new_expr(EXPR_BINARY);
		expr->binary_operator = operator;
		expr->binary_lhs = lhs;
		expr->binary_rhs = rhs;
		lhs = expr;
	}
	return lhs;
}

// expr11 = (name "=" <expr11>) | <expr10>
static expr_t* parse_expr11()
{
	if(match(TKN_IDENT))
	{
		token_t name = expect(TKN_IDENT);
		if(match(TKN_EQ))
		{
			expect(TKN_EQ);
			expr_t* stmt = new_expr(EXPR_ASSIGNMENT);
			stmt->assign_name = name.val_string;
			stmt->assign_rhs = parse_expr11();
			return stmt;
		}
		else
		{
			// If we don't get an '=' after the identifier, revert back to before
			// the identifier and continue to parse and expression.
			prev();
		}
	}
	return parse_expr10();
}

// Parses a statement from the input stream.
// stmt = "return" <expr11> ";"
static stmt_t* parse_statement()
{
	if(match(TKN_RETURN))
	{
		expect(TKN_RETURN);
		expr_t* expr = parse_expr11();
		expect(TKN_SEMICOLON);
		stmt_t* stmt = new_stmt(STMT_RETURN);
		stmt->return_expr = expr;
		return stmt;
	}
	else if(match(TKN_IDENT))
	{
		token_t t = peek();
		if(t.val_string == _("int"))
		{
			// We got a type name, therefore we are parsing a declaration.
			token_t type = expect(TKN_IDENT);
			token_t name = expect(TKN_IDENT);

			expr_t* initializer = NULL;
			if(match(TKN_EQ))
			{
				expect(TKN_EQ);
				initializer = parse_expr11();
			}
			expect(TKN_SEMICOLON);

			stmt_t* stmt = new_stmt(STMT_DECLARE);
			stmt->declare_name = name.val_string;
			stmt->declare_initializer = initializer;
			return stmt;
		}
	}

	// If we didn't match any valid statement, then we are parsing a standalone
	// expression.
	expr_t* expr = parse_expr11();
	expect(TKN_SEMICOLON);
	stmt_t* stmt = new_stmt(STMT_EXPR);
	stmt->standalone_expr = expr;
	return stmt;
}

// Parses a declaration from the input stream.
// decl = "int" identifier "(" ")" "{" { <stmt> } "}"
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

	stmt_t** stmts = NULL;
	while(!match(TKN_R_CURLY))
	{
		stmt_t* stmt = parse_statement();
        sb_push(stmts, stmt);
	}

	expect(TKN_R_CURLY);

	// For now we only support function delcarations.
	// In the future we will support other types of declarations.
	decl_t* decl = new_decl(DECL_FUNC);
	decl->name = name.val_string;
	decl->stmts = stmts;

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

expr_t* _parse_expression(token_t* tokens)
{
	state.tokens = tokens;
	state.ptr = 0;

	return parse_expr11();
}

stmt_t* _parse_statement(token_t* tokens)
{
	state.tokens = tokens;
	state.ptr = 0;

	return parse_statement();
}
#include "lex.h"

// Global state for the lexer.
// The state is reset with each call to 'lex()'.
static struct
{
	char* source;
	int ptr;

	int line;

	token_t* tokens;
} state;

//
// Character utilities.
//

static bool is_whitespace(char c)
{
	return c == 0x20 || c == 0x0A || c == 0x0D || c == 0x09;
}

static bool is_numeric(char c)
{
	return c >= '0' && c <= '9';
}

// Returns true if the given character can be the first character in an identifier
// name, false otherwise.
static bool is_ident_start_char(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

// Returns true if the given character can appear in an identifier name,
// false otherwise.
static bool is_ident_char(char c)
{
	return is_ident_start_char(c) || is_numeric(c);
}

//
// State modifiers.
//

// Returns true if there are more characters to be read from the input,
// false otherwise.
static bool has_next()
{
	return state.source[state.ptr] != '\0';
}

// Returns the next character in the input stream, does not increment
// the pointer.
static char peek()
{
	return state.source[state.ptr];
}

// Returns the next character in the input stream, incremenets the
// internal pointer to point to the next character.
static char next()
{
	return state.source[state.ptr++];
}

// Allocates a new token on the stack with the given type.
static token_t new_token(token_type_t type)
{
	token_t token;
	token.type = type;
	token.val_integer = 0;
	token.val_string = 0;
	return token;
}

// Pushes a new token with the given type to the lexers output stream.
static void emit(token_type_t type)
{
	token_t token = new_token(type);
	sb_push(state.tokens, token);
}

// Pushes an new integer token to the lexers output stream.
static void emit_integer(uint64_t value)
{
	token_t token = new_token(TKN_INTEGER);
	token.val_integer = value;
	sb_push(state.tokens, token);
}

// Pushes a new identifier token to the lexers output stream.
static void emit_identifier(str_t ident)
{
	token_t token = new_token(TKN_IDENT);
	token.val_string = ident;
	sb_push(state.tokens, token);
}

//
// Parse utilities.
//

// Parses an integer literal from the source input.
// If source[ptr] is not a valid numeric charatcer, then the
// output of this function is undefined.
static uint64_t parse_integer()
{
	uint64_t val = 0;

	while(has_next() && is_numeric(peek()))
	{
		char c = next();
		val *= 10;
		val += (uint64_t)(c - '0');
	}
	return val;
}

// Parses an identifier name from the source input.
// If source[ptr] is not a valid identifier character, then the
// output of this function is undefined.
static str_t parse_identifier()
{
	char* start = state.source + state.ptr;

	while(has_next() && is_ident_char(peek()))
	{
		next();
	}

	char* end = state.source + state.ptr;
	int len = end - start;
	return __(start, len);
}

//
// Public API.
//

token_t* lex(char* source)
{
	// Reset state.
	state.source = source;
	state.ptr = 0;
	state.line = 1;
	sb_free(state.tokens);
	state.tokens = NULL;

	// Continue to read tokens until we reach the end of the stream.
	while(has_next())
	{
		char c = peek();

		// Ignore all whitespace.
		if(is_whitespace(c))
		{
			if(next() == '\n')
			{
				// If we found a newline, increment the line counter.
				state.line++;
			}
			continue;
		}

		// We found a number, parse it.
		if(is_numeric(c))
		{
			uint64_t value = parse_integer();
			emit_integer(value);
			continue;
		}

		// We found an identifier, parse it.
		if(is_ident_start_char(c))
		{
			str_t ident = parse_identifier();

			// Check for reserved keywords.
			if(ident == _("return")) { emit(TKN_RETURN); continue; }

			// If we didn't find a reserved keyword, just emit an identifier.
			emit_identifier(ident);
			continue;
		}

		// Handle all "simple" tokens here.
		switch(c)
		{
		case '{': emit(TKN_L_CURLY  ); next(); continue;
		case '}': emit(TKN_R_CURLY  ); next(); continue;
		case '(': emit(TKN_L_PAREN  ); next(); continue;
		case ')': emit(TKN_R_PAREN  ); next(); continue;
		case ';': emit(TKN_SEMICOLON); next(); continue;
		case '-': emit(TKN_MINUS    ); next(); continue;
		case '~': emit(TKN_TILDE    ); next(); continue;
		case '!': emit(TKN_BANG     ); next(); continue;
		case '+': emit(TKN_PLUS     ); next(); continue;
		case '*': emit(TKN_ASTERIX  ); next(); continue;
		case '/': emit(TKN_SLASH    ); next(); continue;
		}

		// If we reach this point of the code, then the given character has not been
		// processed anywhere above, so we assume that it is an unknown character and
		// and throw an error.
		error("unexpected character '%c' at line %d\n", c, state.line);
	}

	// We've reached the end of the stream, so add a terminating token.
	emit(TKN_EOF);

	return state.tokens;
}


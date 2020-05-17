#ifndef _TOKEN_H
#define _TOKEN_H

#include <stdint.h>

#include "str.h"

// Master list of supported token types.
typedef enum
{
	TKN_INTEGER,
	TKN_L_CURLY,
	TKN_R_CURLY,
	TKN_L_PAREN,
	TKN_R_PAREN,
	TKN_SEMICOLON,
	TKN_RETURN,
	TKN_IDENT,
	TKN_EOF
} token_type_t;

// Lookup table for token type names.
static char* token_type_names[] =
{
	"integer",
	"l-curly",
	"r-curly",
	"l-paren",
	"r-paren",
	"semi colon",
	"return",
	"identifier",
	"eof"
};

// Represents a single token in the source input.
typedef struct
{
	token_type_t type;
	union {
		uint64_t val_integer;
		str_t val_string;
	};
} token_t;

#endif

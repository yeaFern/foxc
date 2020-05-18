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
	TKN_MINUS,
	TKN_TILDE,
	TKN_BANG,
	TKN_PLUS,
	TKN_ASTERIX,
	TKN_SLASH,
	TKN_AMP,
	TKN_AMP_AMP,
	TKN_PIPE,
	TKN_PIPE_PIPE,
	TKN_EQ,
	TKN_EQ_EQ,
	TKN_NOT_EQ,
	TKN_LT,
	TKN_LT_LT,
	TKN_LT_EQ,
	TKN_GT,
	TKN_GT_GT,
	TKN_GT_EQ,
	TKN_MODULO,
	TKN_CARET,
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
	"minus",
	"tilde",
	"bang",
	"plus",
	"asterix",
	"slash",
	"ampersand",
	"dbl-amp",
	"pipe",
	"dbl-pipe",
	"equals",
	"dbl-equals",
	"not-equals",
	"less",
	"dbl-less",
	"less-eq",
	"greater",
	"dbl-greater",
	"greater-eq",
	"percent",
	"caret",
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

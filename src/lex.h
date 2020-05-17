#ifndef _LEX_H
#define _LEX_H

#include <stdbool.h>

#include "token.h"
#include "error.h"

// Returns a list of tokens given some source input.
// If the lexer encounters an error, the program will terminate and an
// error message will be printed to the user.
token_t* lex(char* source);

#endif

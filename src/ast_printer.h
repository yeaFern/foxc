#ifndef _AST_PRINTER_H
#define _AST_PRINTER_H

#include <stdio.h>

#include "parser.h"

// Prints the given AST to the given file handle.
void print_ast(FILE* handle, program_t* program);

#endif

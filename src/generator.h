#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <stdio.h>

#include "parser.h"
#include "buf.h"

void generate(FILE* handle, program_t* program);

#endif

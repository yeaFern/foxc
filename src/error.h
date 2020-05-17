#ifndef _ERROR_H
#define _ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>

#define UNHANDLED_CASE() error("unhandled case @ %s:%d\n", __FILE__, __LINE__)

// Fatally exits the program with the given error message.
// NOTE: This function calls exit() and as such, does not return.
_Noreturn void error(char* fmt, ...);

#endif

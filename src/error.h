#ifndef _ERROR_H
#define _ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>

// Fatally exits the program with the given error message.
// NOTE: This function calls exit() and as such, does not return.
_Noreturn void error(char* fmt, ...);

#endif

#ifndef _STR_H
#define _STR_H

#include <string.h>
#include <stdbool.h>

#include "buf.h"

#define _(x) intern_str((x))
#define __(x, y) intern_str_range((x), (y))

// Any occurance of str_t in the code base implies that the given string is
// stored in the intern table, and therefore is safe to compare using '=='.
typedef char* str_t;

// Look up or insert the given string into the intern table.
// The returned pointer will be a valid entry into the intern table.
str_t intern_str(char* str);

// Look up or insert the given string range into the intern table.
// The returned pointer will be a valid entry into the intern table.
str_t intern_str_range(char* buf, int len);

#endif

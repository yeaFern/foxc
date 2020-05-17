#include "str.h"

static str_t* intern_table = NULL;

str_t intern_str(char* str)
{
	// Try and find the string in the intern table.
    for(int i = 0; i < sb_count(intern_table); i++)
    {
		// We must use strcmp here as we cannot be sure that the input
		// string is a valid interned string.
        if(!strcmp(intern_table[i], str))
        {
            return intern_table[i];
        }
    }

	// If we don't have the string in memory, allocate it.
    char* s = malloc(strlen(str) + 1);
    memcpy(s, str, strlen(str) + 1);
    sb_push(intern_table, s);

	// Lazily call the function again to find the string.
    return intern_str(s);
}

str_t intern_str_range(char* buf, int len)
{
	// TODO: This isn't a very nice solution.
    static char temp[1024 * 4];
    strncpy(temp, buf, len);
    temp[len] = '\0';
    return intern_str(temp);
}

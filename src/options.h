#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// E.g. "fiflo --version" etc.
bool options__parse_and_print(const char* const);

#endif

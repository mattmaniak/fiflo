#ifndef OPTIONS_H
#define OPTIONS_H

// Command-line arguments prefixed with '-' or "--" handling.

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "release.h"

// E.g. "fiflo --version" etc.
bool options__parse_and_print(const char* const);

#endif

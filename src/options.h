#ifndef OPTIONS_H
#define OPTIONS_H

// Handling command-line arguments prefixed with '-' or "--".

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "modes.h"
#include "release.h"

// E.g. "fiflo --version" etc.
bool options__parse_and_print(Mod_t* const, int*, const char* const);

#endif

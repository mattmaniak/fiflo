#ifndef ARGS_H
#define ARGS_H

// Dealing with a command line arguments.

#include <linux/limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "modes.h"
#include "options.h"

// Analyze argumets passed to the editor.
bool args__parse(Mod_t* const, int*, char** const);

#endif

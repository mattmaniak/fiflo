#ifndef ARGS_H
#define ARGS_H

// Dealing with the command line arguments.

#include <linux/limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

// Analyzes argumets passed to the editor.
bool args__parse(int, char**);

#endif

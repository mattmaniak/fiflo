#ifndef ARGS_H
#define ARGS_H

// Dealing with a command line arguments.

#ifdef __APPLE__
// https://opensource.apple.com/source/xnu/xnu-4903.221.2/bsd/sys/syslimits.h.auto.html
#include <sys/syslimits.h>
#endif

#ifdef __linux__
// https://github.com/torvalds/linux/blob/master/include/uapi/linux/limits.h
#include <linux/limits.h>
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "modes.h"
#include "options.h"
#include "size.h"

// Analyze argumets passed to the editor.
bool args__parse(Modes* const, int*, char** const);

#endif

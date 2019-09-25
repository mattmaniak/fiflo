#ifndef FIFLO_H
#define FIFLO_H

// Some shit that starts the program.

#include <stdbool.h>

#include "args.h"
#include "v_file.h"
#include "config.h"
#include "file_io.h"
#include "extension.h"
#include "modes.h"
#include "input.h"
#include "syntax.h"
#include "window.h"
#include "options.h"

// Some initializers and the main program loop.
void fiflo__run(int, char** const);

int main(int, char**);

#endif

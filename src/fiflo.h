#ifndef FIFLO_H
#define FIFLO_H

// Some shit that starts the program.

#include <stdbool.h>

#include "args.h"
#include "buffer.h"
#include "config.h"
#include "file.h"
#include "modes.h"
#include "input.h"
#include "window.h"
#include "options.h"

// Some initializers and the main program loop.
void fiflo__run(int, char**);

int main(int, char**);

#endif

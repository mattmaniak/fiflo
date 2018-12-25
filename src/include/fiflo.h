#ifndef FIFLO_H
#define FIFLO_H

// Some shit that starts the program.

#include <stdio.h>
#include <stdlib.h>

// Program parameters, eg. "--help".
void options(const char* arg);

// Some initializers and the main program loop.
void run(const char* arg);

#endif

#ifndef FIFLO_H
#define FIFLO_H

// Some shit that starts the program.

#include <stdio.h>
#include <stdlib.h>

// Program parameters, eg. "--help".
bool options(const char* arg);

// Some initializers and the main program loop.
bool run(const char* arg);

int main(const int argc, const char** argv);

#endif

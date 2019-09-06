#ifndef FFORMATS_H
#define FFORMATS_H

// Opened file format recognition.

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "line.h"

typedef enum
{
    NONE    = 0,
    C       = 1,
    CPP     = 2,
    PYTHON3 = 3
}
fformats_t;

extern const int FFORMATS_EXTENSION;

void fformats__recognize_extension(const char* const);

#endif

#ifndef FEXTENSION_H
#define FEXTENSION_H

// File extension recognition.

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "line.h"

enum
{
    NONE,
    C,
    CPP,
    PYTHON3
};

int fextension__recognize_extension(const char* const);

#endif

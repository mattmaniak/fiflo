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
    PYTHON,
};

// Recognizes an extension of a current file.
int extension__recognize(const char* const);

#endif

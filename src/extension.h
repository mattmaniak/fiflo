#ifndef FEXTENSION_H
#define FEXTENSION_H

// File extension recognition.

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "line.h"

enum
{
    EXTENSION__NONE,
    EXTENSION__C,
    EXTENSION__PYTHON,
    EXTENSION__HTML
};

// Recognizes an extension of a current file.
char* extension__recognize(char* const);

#endif

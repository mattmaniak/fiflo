#ifndef EXTENSION_H
#define EXTENSION_H

// File extension recognition.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include <string.h>

#include "buffer.h"
#include "config.h"
#include "extension.h"
#include "ui.h"

#define SYNTAX__MAX_KEYWORDS 256

typedef struct
{
    char keyword[16];
    int  color;
}
Syntax__Kwrd_t;

typedef struct
{
    Syntax__Kwrd_t Keywords[256];
    idx_t          kwrds_idx;
}
Syntax_t;

// Loads the file with defined syntax highlighting colors.
bool syntax__load(Syntax_t*, const int);

// Checks and renders the word if it's color should change.
idx_t syntax__paint_word(const Syntax_t* const, Line_t*, idx_t);

#endif

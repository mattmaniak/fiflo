#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include <string.h>

#include "buffer.h"
#include "config.h"
#include "extension.h"
#include "ui.h"

#define SYNTAX__MAX_KWRDS_IN_FILE 256
#define SYNTAX__MAX_KWRD_LEN      16

typedef struct
{
    char keyword[SYNTAX__MAX_KWRD_LEN];
    int  color;
}
Syntax__Kwrd_t;

typedef struct
{
    Syntax__Kwrd_t Keywords[SYNTAX__MAX_KWRDS_IN_FILE];
    size_t         kwrds_amount;
}
Syntax_t;

// Loads a file with defined syntax highlighting colors.
bool syntax__load(Syntax_t* const, const int);

/* Sorts words descending by a length. It prevents glitching words if there is
   e.g "int" before "int32_t" by wrong double painting. */
void syntax__sort(Syntax_t* const);

// Checks and renders a word if it's color should change.
size_t syntax__paint_word(const Syntax_t* const, const Conf_t* const, Line_t*,
                         const size_t, size_t);

#endif

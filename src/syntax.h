#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#include <string.h>

#include "v_file.h"
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
syntax__Keyword;

typedef struct
{
    syntax__Keyword keywords[SYNTAX__MAX_KWRDS_IN_FILE];
    size_t          keywords_amount;
}
Syntax;

// Loads a file with defined syntax highlighting colors.
bool syntax__load(Syntax* const, const char* const);

/* Sorts words descending by a length. It prevents glitching words if there is
   e.g "int" before "int32_t" by wrong double painting. */
void syntax__sort(Syntax* const);

// Checks and renders a word if it's color should change.
size_t syntax__paint_word(const Syntax* const, const Config* const, Line*,
                         const size_t, size_t);

#endif

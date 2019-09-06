#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <stdbool.h>
#include <string.h>

#include "buffer.h"

typedef struct
{
    char (*red_words)[];
    char (*green_words)[];
    char (*yellow_words)[];
    char (*blue_words)[];
    char (*magenta_words)[];
    char (*cyan_words)[];
    char (*white_words)[];
    char (*bright_red_words)[];
    char (*bright_green_words)[];
    char (*bright_yellow_words)[];
    char (*bright_blue_words)[];
    char (*bright_magenta_words)[];
    char (*bright_cyan_words)[];
    char (*bright_white_words)[];
    char (*bright_black_words)[];
}
Highlighter__words_colors_t;

// Checks and renders the word if it's color should change.
idx_t highlighter__paint_word(Line_t*, idx_t);

#endif

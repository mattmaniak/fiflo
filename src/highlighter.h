#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <stdbool.h>
#include <string.h>

#include "buffer.h"

bool highlighter__paint_word(Line_t*, idx_t*);

#endif

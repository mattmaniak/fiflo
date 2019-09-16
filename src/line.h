#ifndef LINE_H
#define LINE_H

// The structure that represents a line of a text.

#include <stdint.h>

typedef uint32_t idx_t;

typedef struct
{
    char* txt;
    idx_t len;

    idx_t _padding;
}
Line_t;

#endif

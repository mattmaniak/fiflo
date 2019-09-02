#ifndef LINE_H
#define LINE_H

#include <stdint.h>

typedef uint32_t idx_t;

typedef struct
{
    char* text;
    idx_t length;

    idx_t _padding;
}
Line_t;

#define LINE__TYPE_SZ sizeof(Line_t)

#endif

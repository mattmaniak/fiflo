#ifndef LINE_H
#define LINE_H

// The structure that represents a line of a text.

typedef struct
{
    char*  txt;
    size_t len;

    const size_t _padding;
}
Line_t;

#endif

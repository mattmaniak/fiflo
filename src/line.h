#ifndef LINE_H
#define LINE_H

// The structure that represents a line of a text.

typedef struct
{
    char*  txt;
    size_t length;

    const size_t _padding;
}
Line;

#endif

#ifndef MEMORY_H
#define MEMORY_H

// Allocation/freeing the memory blocks that holds the text and it's metadata.

#include <string.h>

#define NEXT 1
#define PREV 1

// Allocs next memory block for chars in a line if needed.
bool memory__extend_line(Buff_t*, const idx_t);

// Works as the function above but shrinks memblocks.
bool memory__shrink_current_line(Buff_t*);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
bool memory__shrink_prev_line(Buff_t*);

// Allocs memory for the next line.
bool memory__extend_lines_array(Buff_t*);

// With pointers that contains: lines and their lenghts.
bool memory__shrink_lines_array(Buff_t*);

// Shifts the lines down.
bool memory__copy_lines_forward(Buff_t*);

// Shifts the lines up.
bool memory__copy_lines_backward(Buff_t*);

#endif

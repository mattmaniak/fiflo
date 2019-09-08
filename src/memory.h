#ifndef MEMORY_H
#define MEMORY_H

// Allocation/freeing memory blocks those holds a text and metadata.

#include <string.h>

#include "buffer.h"

// Alloc a next memory block for chars in a line if needed.
bool memory__extend_line(Buff_t*, const idx_t);

// Work as the function above but shrinks memblocks.
bool memory__shrink_current_line(Buff_t*);

/* When the enter is hit with the shifted cursor, a previous line will be
   shrinked. */
bool memory__shrink_prev_line(Buff_t*);

// Alloc a memory for next lines.
bool memory__extend_lines_array(Buff_t*);

// Shrink a memory if is not needed for lines.
bool memory__shrink_lines_array(Buff_t*);

// Shift lines down.
bool memory__copy_lines_forward(Buff_t*);

// Shift lines up.
bool memory__copy_lines_backward(Buff_t*);

#endif

#ifndef MEMORY_H
#define MEMORY_H

// Allocation/freeing the memory blocks that holds the text and it's metadata.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Allocs next memory block for chars in a line if needed.
bool memory_extend_line(Buff_t* Buff, idx_t line_i);

// Works as the function above but shrinks memblocks.
bool memory_shrink_act_line(Buff_t* Buff);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
bool memory_shrink_prev_line(Buff_t* Buff);

// Allocs memory for the next line.
bool memory_extend_lines_array(Buff_t* Buff);

// With pointers that contains: lines and their lenghts.
bool memory_shrink_lines_array(Buff_t* Buff);

// Shifts the lines down.
bool memory_copy_lines_forward(Buff_t* Buff);

// Shifts the lines up.
bool memory_copy_lines_backward(Buff_t* Buff);

#endif

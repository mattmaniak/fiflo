#ifndef MEMORY_H
#define MEMORY_H

// Allocation/freeing the memory blocks that holds the text and it's metadata.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Allocs next memory block for chars in a line if needed.
bool memory__extend_line(Buff_t* Buffer, idx_t line_i);

// Works as the function above but shrinks memblocks.
bool memory__shrink_act_line(Buff_t* Buffer);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
bool memory__shrink_prev_line(Buff_t* Buffer);

// Allocs memory for the next line.
bool memory__extend_lines_array(Buff_t* Buffer);

// With pointers that contains: lines and their lenghts.
bool memory__shrink_lines_array(Buff_t* Buffer);

// Shifts the lines down.
bool memory__copy_lines_forward(Buff_t* Buffer);

// Shifts the lines up.
bool memory__copy_lines_backward(Buff_t* Buffer);

#endif

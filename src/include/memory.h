#ifndef MEMORY_H
#define MEMORY_H

// Allocation/freeing the memory blocks that holds the text and it's metadata.

#include <stdio.h>
#include <stdlib.h>

// Checks if passed pointer is NULL. If yes - frees memory and exits.
void chk_ptr(Buff_t* Buff, void* ptr, const char* err_msg);

// Allocs next memory block for chars in a line if needed.
char* extend_line_mem(Buff_t* Buff, idx_t line_i);

// Works as the function above but shrinks memblocks.
char* shrink_act_line_mem(Buff_t* Buff);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
char* shrink_prev_line_mem(Buff_t* Buff);

// Allocs memory for the next line.
Buff_t* extend_lines_array_mem(Buff_t* Buff);

// With pointers that contains: lines and their lenghts.
Buff_t* shrink_lines_array_mem(Buff_t* Buff);

// Shifts the lines down.
Buff_t* copy_lines_mem_forward(Buff_t* Buff);

// Shifts the lines up.
Buff_t* copy_lines_mem_backward(Buff_t* Buff);

static const struct
{
	void    (*chk_ptr)(Buff_t*, void*, const char*);
	char*   (*extend_line_mem)(Buff_t*, idx_t);
	char*   (*shrink_act_line_mem)(Buff_t*);
	char*   (*shrink_prev_line_mem)(Buff_t*);
	Buff_t* (*extend_lines_array_mem)(Buff_t*);
	Buff_t* (*shrink_lines_array_mem)(Buff_t*);
	Buff_t* (*copy_lines_mem_forward)(Buff_t*);
	Buff_t* (*copy_lines_mem_backward)(Buff_t*);
}
memory =
{
	chk_ptr,
	extend_line_mem,
	shrink_act_line_mem,
	shrink_prev_line_mem,
	extend_lines_array_mem,
	shrink_lines_array_mem,
	copy_lines_mem_forward,
	copy_lines_mem_backward
};

#endif

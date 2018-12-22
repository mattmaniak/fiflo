#ifndef MEMORY_H
#define MEMORY_H

// Allocation/freeing the memory blocks that holds the text and it's metadata.

#include <stdio.h>
#include <stdlib.h>

// Checks if passed pointer is NULL. If yes - frees memory and exits.
void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);

// Allocs next memory block for chars in a line if needed.
char* extend_line_mem(f_mtdt* Buff, buff_t line_i);

// Works as the function above but shrinks memblocks.
char* shrink_act_line_mem(f_mtdt* Buff);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
char* shrink_prev_line_mem(f_mtdt* Buff);

// Allocs memory for the next line.
f_mtdt* extend_lines_array_mem(f_mtdt* Buff);

// With pointers that contains: lines and their lenghts.
f_mtdt* shrink_lines_array_mem(f_mtdt* Buff);

// Shifts the lines down.
f_mtdt* copy_lines_mem_forward(f_mtdt* Buff);

// Shifts the lines up.
f_mtdt* copy_lines_mem_backward(f_mtdt* Buff);

static const struct
{
	void    (*chk_ptr)(f_mtdt*, void*, const char*);
	char*   (*extend_line_mem)(f_mtdt*, buff_t);
	char*   (*shrink_act_line_mem)(f_mtdt*);
	char*   (*shrink_prev_line_mem)(f_mtdt*);
	f_mtdt* (*extend_lines_array_mem)(f_mtdt*);
	f_mtdt* (*shrink_lines_array_mem)(f_mtdt*);
	f_mtdt* (*copy_lines_mem_forward)(f_mtdt*);
	f_mtdt* (*copy_lines_mem_backward)(f_mtdt*);
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

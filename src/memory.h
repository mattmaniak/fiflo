#ifndef MEMORY_H
#define MEMORY_H

// Allocation/freeing the memory blocks that holds the text and it's metadata.

#include <stdio.h>
#include <stdlib.h>

/* Note: They are not prototypes or pointers to these specific funtions. They
are pointers to funtions with certain return type and parameters including it's
types. For better readability they are named as their definitions. */
typedef const struct
{
	// Checks if passed pointer is NULL. If yes - frees memory and exits.
	void (*chk_ptr)(Buff_t*, void*, const char*);

	// Allocs next memory block for chars in a line if needed.
	int (*extend_line_mem)(Buff_t*, idx_t);

	// Works as the function above but shrinks memblocks.
	int (*shrink_act_line_mem)(Buff_t*);

	// When the enter is hitted with shifted cursor, previous line will be shrinked.
	int (*shrink_prev_line_mem)(Buff_t*);

	// Allocs memory for the next line.
	int (*extend_lines_array_mem)(Buff_t*);

	// With pointers that contains: lines and their lenghts.
	int (*shrink_lines_array_mem)(Buff_t*);

	// Shifts the lines down.
	int (*copy_lines_mem_forward)(Buff_t*);

	// Shifts the lines up.
	int (*copy_lines_mem_backward)(Buff_t*);
}
namespace_memory;

extern namespace_memory memory;

#endif

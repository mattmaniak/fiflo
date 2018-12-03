#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>

#define INDEX 1

// Bytes of the memory width. Depends on an architecture. Eg. 8 bytes.
#define ADDR_SZ sizeof(Buff->text)

// Aligned initial memblk for a new line.
#define INIT_MEMBLK ADDR_SZ

// Must be >= 16 and dividable by 8.
#define MEMBLK 128

// Frees everything and exits with status code.
_Noreturn void free_buff_exit(f_mtdt* Buff, const bool code);

// Checks if passed pointer is NULL. If yes - frees memory and exits.
void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);

// Frees and sets the pointer to NULL.
void safer_free(void* ptr);

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
f_mtdt* copy_lines_forward(f_mtdt* Buff);

// Shifts the lines up.
f_mtdt* copy_lines_backward(f_mtdt* Buff);

#endif

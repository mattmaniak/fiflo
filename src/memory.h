#ifndef MEMORY_H
#define MEMORY_H

// Bytes of the memory width. Depends on an architecture. Eg. 8 bytes.
#define ADDR_SZ sizeof(Buff->text)

// Must be >= 16 and dividable by 8.
#define MEMBLK 16

// Aligned initial memblk for a new line.
#define INIT_MEMBLK ADDR_SZ

// Allocs next memory block for chars in a line if needed.
char* extend_line(f_mtdt* Buff, buff_t line_i);

// Works as the function above but shrinks memblocks.
char* shrink_act_line(f_mtdt* Buff);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
char* shrink_prev_line(f_mtdt* Buff);

// Allocs memory for the next line.
f_mtdt* extend_lines_array(f_mtdt* Buff);

// With pointers that contains: lines and their lenghts.
f_mtdt* shrink_lines_array(f_mtdt* Buff);

// Shifts the lines down.
f_mtdt* copy_lines_forward(f_mtdt* Buff);

// Shifts the lines up.
f_mtdt* copy_lines_backward(f_mtdt* Buff);

#endif

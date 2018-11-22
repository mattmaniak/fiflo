#ifndef MEMORY_H
#define MEMORY_H

// TODO: COMMENT
#define ADDR_SZ sizeof(Buff->text)

// Must be >= 16 and dividable by 8.
#define MEMBLK_SZ 16

// Align the memory block to the architecture (4/8 bytes division).
#define MEMBLK ((MEMBLK_SZ / ADDR_SZ) * ADDR_SZ)

// Aligned initial memblk for a new line.
#define INIT_MEMBLK ADDR_SZ

// Allocs next memory block for chars in a line if needed.
char* extend_line(f_mtdt* Buff, buff_t line);

// Works as the function above but shrinks memblocks.
char* shrink_act_line(f_mtdt* Buff);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
char* shrink_prev_line(f_mtdt* Buff);

// Allocs memory for the next line.
f_mtdt* extend_lines_array(f_mtdt* Buff);

// With pointers that contains: lines and their lenghts.
f_mtdt* shrink_lines_array(f_mtdt* Buff);

#endif

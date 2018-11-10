#ifndef MEMORY_H
#define MEMORY_H

// Must be >= 16 and dividable by 8.
#define MEMBLK ((128 / sizeof(Buff->text)) * sizeof(Buff->text))

// Aligned initial memblk for a new line.
#define INIT_MEMBLK sizeof(Buff->text)

// Allocs next memory block for chars in a actent line if needed.
f_mtdt* extend_act_line(f_mtdt* Buff);

// Works as above function but is used with backspace pressed.
f_mtdt* extend_prev_line(f_mtdt* Buff);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
f_mtdt* shrink_prev_line(f_mtdt* Buff);

// Works as the function above but shrinks memblocks.
f_mtdt* shrink_act_line(f_mtdt* Buff);

// Allocs memory for the next line.
f_mtdt* extend_lines_array(f_mtdt* Buff);

// With pointers that contains: lines and their lenghts.
f_mtdt* shrink_lines_array(f_mtdt* Buff);

#endif


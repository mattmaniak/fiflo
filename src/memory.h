#ifndef MEMORY_H
#define MEMORY_H

#define MEMBLK 128 // Must be >= 16 and dividable by 8.

// Allocs next memory block for chars in a actent line if needed.
meta* extend_act_line(meta* Dt);

// Works as above function but is used with backspace pressed.
meta* extend_prev_line(meta* Dt);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
meta* shrink_prev_line(meta* Dt);

// Works as the function above but shrinks memblocks.
meta* shrink_act_line(meta* Dt);

// Allocs memory for the next line.
meta* extend_lines_array(meta* Dt);

// With pointers that contains: lines and their lenghts.
meta* shrink_lines_array(meta* Dt);

#endif


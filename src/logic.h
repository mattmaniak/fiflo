#ifndef LOGIC_H
#define LOGIC_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <linux/limits.h>

#define NTERM_SZ  1
#define NEG      -1
#define NTERM     0
#define CTRL_D    4
#define TAB       9
#define LF        10
#define CTRL_X    24
#define ESCAPE    27 // '\033' - ANSI escape code initial sequence.
#define BACKSPACE 127

// From key.
extern meta* non_control_chr(char key, meta* Dt);
extern meta* linefeed(meta* Dt);
extern meta* backspace(meta* Dt);
extern meta* ctrl_g(meta* Dt);
extern meta* ctrl_h(meta* Dt);

// Sets the filename. If basename is passed as arg actent path will be added.
meta* set_fname(const char* arg, meta* Dt);

// Reads the file using "recognize_char".
meta* read_file(meta* Dt);

// Puts the whole text to the file.
void save_file(meta* Dt);

// Allocs next memory block for chars in a actent line if needed.
meta* extend_act_ln_mem(meta* Dt);

// When the enter is hitted with shifted cursor, previous line will be shrinked.
meta* shrink_prev_ln_mem(meta* Dt);

// Works as the function above but shrinks memblocks.
meta* shrink_act_ln_mem(meta* Dt);

// Allocs memory for the next line.
meta* extend_lines_array(meta* Dt);

// With pointers that contains: lines and their lenghts.
meta* shrink_lines_array(meta* Dt);

// Moves the text when the cursor is moved left and char is pressed.
meta* shift_text_horizonally(char direction, meta* Dt);
#endif


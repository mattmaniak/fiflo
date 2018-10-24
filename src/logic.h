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

// These keys moves the cursor...
#define CTRL_G 7 // left.
#define CTRL_H 8 // right.

// From key.
extern meta* non_control_chr(char key, meta* Dt);
extern meta* linefeed(meta* Dt);
extern meta* backspace(meta* Dt);
extern meta* ctrl_g(meta* Dt);
extern meta* ctrl_h(meta* Dt);

// Sets the filename. If basename is passed as arg current path will be added.
meta* set_fname(const char* arg, meta* Dt);

// Reads the file using "recognize_char".
meta* read_file(meta* Dt);

// Puts the whole text to the file.
void save_file(meta* Dt);

// Allocs next memory block for chars in a current line if needed.
meta* add_mem_for_chrs(meta* Dt);

// Works as the function above but shrinks memblocks.
meta* free_mem_for_chrs(meta* Dt);

// Allocs memory for the next line.
meta* alloc_mem_for_lns(meta* Dt);

// 
meta* free_mem_for_lns(meta* Dt);

// Moves the text when the cursor is moved left and char is pressed.
meta* shift_txt_horizonally(char direction, meta* Dt);

// Knows what to do next with pressed key or combination. Based on ASCII.
meta* recognize_key(char key, meta* Dt);
#endif


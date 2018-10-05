#ifndef API_H
#define API_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <linux/limits.h>

#define NTERM_SZ  1
#define SLASH_SZ  1
#define NEG      -1
#define NTERM     0
#define CTRL_D    4
#define TAB       9
#define LF        10
#define CTRL_X    24
#define ESCAPE    27  // '\033'
#define BACKSPACE 127

// These keys moves the cursor...
#define CTRL_B 2  // down.
#define CTRL_G 7  // left.
#define CTRL_H 8  // right.
#define CTRL_Y 25 // up.

#define LAST_CHAR Dat->ln_len - NTERM_SZ
#define CURR_LN   Dat->txt[Dat->lns]
#define LN_ABOVE  Dat->txt[Dat->lns - 1]

void set_fname(meta* Dat, const char* passed); // Optionally get current dir.
meta* read_file(meta* Dat);
void save_file(meta* Dat);

meta* dealloc_block(meta* Dat); // Check and free memblock.
meta* alloc_block(meta* Dat, char mode); // Check and alloc block of memory.

meta* shift_txt(meta* Dat, char direction);
meta* add_char(meta* Dat, char key); // If a char is not the BACKSPACE.
meta* reco_key(meta* Dat, char key); // Recognize type of a passed char.
#endif


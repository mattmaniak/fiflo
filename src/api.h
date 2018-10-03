#ifndef API_H
#define API_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <linux/limits.h>

#define NEG      -1
#define NTERM     0
#define CTRL_D    4
#define TAB       9
#define LF        10
#define CTRL_X    24
#define ESCAPE    27
#define BACKSPACE 127

// These keys moves the cursor...
#define CTRL_B 2  // down.
#define CTRL_G 7  // left.
#define CTRL_H 8  // right.
#define CTRL_Y 25 // up.

#define LAST_CHAR dt->ln_len - NTERM_SZ
#define CURR_LN   dt->txt[dt->lns]
#define LN_ABOVE  dt->txt[dt->lns - 1]

meta* dealloc_block(meta* dt); // Check and free memblock.
meta* alloc_block(meta* dt, char mode); // Check and alloc block of memory.

meta* shift_txt(meta* dt, char direction);
meta* add_char(meta* dt, char key); // If a char is not the BACKSPACE.
meta* recognize_char(meta* dt, char key); // Recognize type of a passed char.
#endif


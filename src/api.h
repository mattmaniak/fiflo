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

extern meta* ctrlh(meta* Dat);
extern meta* ctrlg(meta* Dat);
extern meta* ctrly(meta* Dat);
extern meta* ctrlb(meta* Dat);
extern meta* backspace(meta* Dat);

void fnameset(meta* Dat, const char* arg); // Optionally get current dir.
meta* readfile(meta* Dat);
void savefile(meta* Dat);

meta* freeblk(meta* Dat); // Check and free memblock.
meta* allocblk(meta* Dat, char mode); // Check and alloc block of memory.

meta* txtshift(meta* Dat, char direction);
meta* addchar(meta* Dat, char key); // If a char is not the BACKSPACE.
meta* keymap(meta* Dat, char key); // Recognize type of a passed char.
#endif


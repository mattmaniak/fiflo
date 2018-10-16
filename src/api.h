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

extern meta* ctrlh(meta* Dt);
extern meta* ctrlg(meta* Dt);
extern meta* backspace(meta* Dt);
extern meta* linefeed(meta* Dt);
extern meta* addchar(char key, meta* Dt);

void fnameset(const char* arg, meta* Dt); // Optionally get current dir.
meta* readfile(meta* Dt);
void savefile(meta* Dt);

meta* chrsalloc(meta* Dt);
meta* txtshift(char direction, meta* Dt);
meta* keymap(meta* Dt, char key); // Recognize type of a passed char.
#endif


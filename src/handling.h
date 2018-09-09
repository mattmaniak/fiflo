#ifndef HANDLING_H
#define HANDLING_H // Non-seen API things that makes text && misc. things works.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <linux/limits.h>

// Required keymap.
#define NEG -1
#define NTERM 0
#define CTRL_D 4
#define TAB 9
#define LF 10
#define CTRL_X 24
#define ESCAPE 27
#define BACKSPACE 127

#define INDEX 1
#define NTERM_SZ 1
#define UPLN dt.lns - 1

void fnameset(buf dt, const char* passed); // Optionally get current dir.
buf readfile(buf dt);
void savefile(buf dt);

_Noreturn void freeallexit(buf dt, _Bool code); // Free everything from heap.
buf freeblk(buf dt); // Check and free memblock.
buf allocblk(buf dt, char mode); // Check and alloc block of memory.

buf charadd(buf dt, char key); // If a char is not the BACKSPACE.
void keyboardshort(buf dt, char key); // Done if pressed key is shortcut.
buf recochar(buf dt, char key); // Recognize type of a passed char.

#endif


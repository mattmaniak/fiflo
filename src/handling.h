#ifndef HANDLING_H
#define HANDLING_H // Non-seen API things that makes text && misc. things works.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <linux/limits.h>

// Required keymap.
#define NEG -1
#define NTERM 0
#define CTRL_D 4
#define BELL 7
#define BACKSPACE 8
#define TAB 9
#define LF 10
#define VTAB 11
#define FORMFEED 12
#define CR 13
#define CTRL_X 24
#define ESCAPE 27

#define INDEX 1
#define NTERM_SZ 1

void fnameset(buff dt, const char* passed);
buff readfile(buff dt);
void savefile(buff dt);

buff allocblk(buff dt, char mode);
buff freeblk(buff dt);
void freeallexit(buff dt, _Bool code);

buff charadd(buff dt, char key);
buff keyboard_shortcut(buff dt, char key);
buff recochar(buff dt, char key);

#endif


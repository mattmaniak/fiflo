#ifndef HANDLING_H
#define HANDLING_H // Non-seen API things that makes text && misc. things works.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <linux/limits.h>

// Required keymap.
#define NEG -1
#define NTERM 0
#define CTRL_D 4
#define BELL 7
#define TAB 9
#define LF 10
#define VTAB 11
#define FORMFEED 12
#define CR 13
#define CTRL_X 24
#define ESCAPE 27
#define BACKSPACE 127

#define INDEX 1
#define NTERM_SZ 1
#define UPLN dt.lns - 1

void fnameset(buf dt, const char* passed);
buf readfile(buf dt);
void savefile(buf dt);

buf allocblk(buf dt, char mode);
buf freeblk(buf dt);
void freeallexit(buf dt, _Bool code);

buf charadd(buf dt, char key);
void keyboard_shortcut(buf dt, char key);
buf recochar(buf dt, char key);

#endif


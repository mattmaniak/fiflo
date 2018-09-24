#ifndef API_H
#define API_H // Magic non-seen things that makes text && misc. them work.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <linux/limits.h>

#define NEG -1
#define NTERM 0
#define CTRL_D 4
#define TAB 9
#define LF 10
#define CTRL_X 24
#define ESCAPE 27
#define BACKSPACE 127

#define LASTCHR dt->chrs_ln - NTERM_SZ
#define CURRLN dt->txt[dt->lns]
#define UPLN dt->txt[dt->lns - 1]

extern term_t termgetsz(buf* dt, char axis); // From "render.h";

void fnameset(buf* dt, const char* passed); // Optionally get current dir.
buf* readfile(buf* dt);
void savefile(buf* dt);

_Noreturn void freeallexit(buf* dt, _Bool code); // Free everything from heap.
buf* freeblk(buf* dt); // Check and free memblock.
buf* allocblk(buf* dt, char mode); // Check and alloc block of memory.

buf* charadd(buf* dt, char key); // If a char is not the BACKSPACE.
buf* recochar(buf* dt, char key); // Recognize type of a passed char.

#endif


#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <termios.h>
#include <unistd.h>

#define CURRENT  1
#define INDEX    1
#define NTERM_SZ 1
#define NTERM    0
#define LF       10

#define MAX_LNS      USHRT_MAX - 1 // - 1 is index.
#define MAX_CHRS     MAX_LNS - 1   // Same as above but with the terminator.
#define MEMBLK       8             // Must be > 1 and should be % 8 = 0.
#define STRLEN_BUF_T 6             // Eg. Strlen("255"). Must be > term_sz('X').

typedef uint16_t buf_t;  // Only for amount indicators.
typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

#pragma pack(push, 1)
typedef struct
{
	FILE*  txtf;   // File handle.
	char*  fname;  // Full filename. Eg. /home/user/basename.
	char** txt;    // Text buffer. Eg. txt[lns][chrs].
	buf_t  chrs;   // All chars index.
	buf_t  lns;    // Lines index.
	buf_t* ln_len; // Chars in the current line (index).
	buf_t  cusr_x; // User's cursor position in reversed X.
	buf_t  cusr_y; // Same as above but vertically. Bigger value - more up.
}
meta;
#pragma pack(pop)

#define LAST_LN         Dt->txt[Dt->lns]
#define LAST_LN_LEN     Dt->ln_len[Dt->lns]
#define PRE_LAST_LN     Dt->txt[Dt->lns - NTERM_SZ]
#define PRE_LAST_LN_LEN Dt->ln_len[Dt->lns - 1]
#define CURR_LN         Dt->txt[Dt->lns - Dt->cusr_y]
#define CURR_LN_LEN     Dt->ln_len[Dt->lns - Dt->cusr_y]

// API
extern meta* keymap(meta* Dt, char key);
extern void fnameset(const char* arg, meta* Dt);
extern meta* readfile(meta* Dt);

// RENDER
extern void flushwin(meta* Dt);
extern void window(meta* Dt);

_Noreturn void freeallexit(_Bool code, meta* Dt);

void ignoresig(int nothing);
// Checks if the passed pointer is NULL. If yes - frees memory and exits.
void ptrcheck(void* ptr, const char* err_msg, meta* Dt);

void options(const char* arg); // Eg. -v, --version et al infos.
char getch(void);              // Getchar without ENTER confirmation.

meta* init(const char* arg, meta* Dt);
_Noreturn void run(const char* arg); // Contains program loop.
int main(int argc, char** argv);
#endif


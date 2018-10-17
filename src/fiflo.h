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
}
meta;
#pragma pack(pop)

#define CURR_LN         Dt->txt[Dt->lns]
#define CURR_LN_LEN     Dt->ln_len[Dt->lns]
#define PRE_CURR_LN     Dt->txt[Dt->lns - 1]
#define PRE_CURR_LN_LEN Dt->ln_len[Dt->lns - 1]

// From api.
extern meta* recognize_key(char key, meta* Dt);
extern meta* set_fname(const char* arg, meta* Dt);
extern meta* read_file(meta* Dt);

// From render.
extern void flush_window(meta* Dt);
extern void window(meta* Dt);

// Frees everything and exits with status code.
_Noreturn void free_all_exit(_Bool code, meta* Dt);

// Signal catcher that does nothing.
void ignore_sig(int nothing);

// Checks if passed pointer is NULL. If yes - frees memory and exits.
void chk_ptr(void* ptr, const char* err_msg, meta* Dt);

// Program parameters, eg. "--help".
void options(const char* arg);

// Reads one char wthout confirming by the ENTER key.
char getch(void);

// Initialize all Dt structure members.
meta* init(const char* arg, meta* Dt);

// Some initializers and the main program loop.
_Noreturn void run(const char* arg);

int main(int argc, char** argv);
#endif


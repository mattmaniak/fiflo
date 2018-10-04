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

#define MAX_LNS      USHRT_MAX - 1 // - 1 is index.
#define MAX_CHRS     MAX_LNS - 1   // Same as above but with the terminator.
#define MEMBLK       512           // Must be > 1 and should be % 8 = 0.
#define STRLEN_BUF_T 5             // Eg. Strlen("255"). Must be > term_sz('X').

typedef uint16_t buf_t;  // Only for amount indicators.
typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

// TODO: REPLACE ln_len WITH ln_len[LNS].
#pragma pack(push, 1)
typedef struct
{
	FILE*  txtf;    // File handle.
	char*  fname;   // Full filename. Eg. /home/user/basename.
	char** txt;     // Text buffer. Eg. txt[lns][chrs].
	buf_t* ln_len;  // Chars in the current line (index).
	buf_t  chrs;    // All chars index.
	buf_t  lns;     // Lines index.
	buf_t  cusr_x;  // User's cursor position in reversed X.
	buf_t  cusr_y;  // Same as above but vertically. Bigger value - more up.
}
meta;
#pragma pack(pop)

_Noreturn void free_all_exit(meta* Dat, _Bool code);
void sig_handler(int nothing);
void check_ptr(meta* Dat, void* ptr, const char* errmsg); // Check if ptr is NULL.

void options(const char* arg); // Eg. -v, --version et al infos.
char getch(void); // Getchar without confirming by ENTER.

meta* init(meta* Dat, const char* passed);
_Noreturn void run(const char* passed); // Contains program loop.
int main(int argc, char** argv);
#endif


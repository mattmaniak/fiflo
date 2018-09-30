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

#define CURRENT 1
#define INDEX 1
#define NTERM_SZ 1

#define MAX_LNS USHRT_MAX - 1 // - 1 is index.
#define MAX_CHRS MAX_LNS      // Same as above.
#define MEMBLK 512            // Must be > 1 and should be % 8 = 0.
#define STRLEN_BUF_T 5        // Eg. USHRT_MAX (65535) => 5 as the strlen.  

typedef uint16_t buf_t; // Only for amount indicators.
typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

#pragma pack(push, 1)
typedef struct
{
	FILE*  txtf;    // File handle.
	char*  fname;   // Full filename, eg. /home/user/basename.
	char** txt;     // Text buffer. Eg. txt[lns][chrs].
	buf_t  chrs;    // All chars index.
	buf_t  chrs_ln; // Chars in the current line (index).
	buf_t  lns;     // Lines index.
	buf_t  cusr_x;  // User's cursor position in reversed X.
	buf_t  cusr_y;  // Same as above but vertically. Bigger value - more up.
}
buf;
#pragma pack(pop)

_Noreturn void freeallexit(buf* dt, _Bool code); // Free everything from heap.
void sighandler(int nothing);
void checkptr(buf* dt, void* ptr, const char* errmsg); // Check if ptr is NULL.

void options(const char* arg); // Eg. -v, --version et al infos.
char nixgetch(void); // Getchar without confirming by ENTER.

buf* init(buf* dt, const char* passed);
_Noreturn void run(const char* passed); // Contains program loop.
int main(int argc, char** argv);

#endif


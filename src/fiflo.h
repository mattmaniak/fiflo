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

// Misc. placeholders.
#define CURRENT 1
#define INDEX 1
#define NTERM_SZ 1

#define MAX_LNS USHRT_MAX - 1 // - 1 is index.
#define MAX_CHRS MAX_LNS
#define MEMBLK 512 // For optimal allocation Must be > 1 and should be % 8 = 0.
#define STRLENBUFF 5 // Eg. USHRT_MAX (65535) => 5 as the strlen.  

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".
typedef uint16_t buf_t; // Only for amount indicators.

#pragma pack(push, 2) // sizeof(meta) = 30. Align to 32 that can be divided by 8.
typedef struct
{
	// Metadata.
	FILE* txtf;
	char* fname; // Full filename, eg. /home/user/basename
	// Main buffer.
	char** txt; // Eg. txt[lns][chrs].
	// Indicators.
	buf_t chrs; // All chars index.
	buf_t chrs_ln; // Chars in the current line (index).
	buf_t lns; // Lines index.
	// User's cursor position.
	term_t cusr_x;
	term_t cusr_y;	
}
meta;
#pragma pack(pop)

void sighandler(int nothing);
void checkptr(meta* dt, void* ptr, const char* errmsg); // Check if ptr is NULL.

void options(const char* arg); // Eg. -v, --version et al infos.
char nixgetch(void); // Getchar without confirming by ENTER.

meta* init(meta* dt, const char* passed);
_Noreturn void run(const char* passed); // Contains program loop.
int main(int argc, char** argv);

#endif


#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/file.h>
#include <unistd.h>
#include <termios.h>

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".
typedef uint16_t buff_t; // Only for amount indicators.

#define MEMBLK 8 // For optimal mallocation Must be > 1 and should be % 8 = 0.
#define MAX_LNS USHRT_MAX - 1
#define MAX_CHRS MAX_LNS - 1 // 1 for NULL

typedef struct
{
	char* fname; // Full filename, eg. /home/user/basename
	char** txt; // Eg. txt[lns][chrs].
	buff_t chrs;
	buff_t chrs_ln; // Chars in the current line.
	buff_t lns;
}
buff;

void sigignore(int nothing);
void checkptr(buff dt, void* ptr, const char* errmsg);
void argc_check(int arg_count);
void options(const char* arg);
void run(const char* passed);
char nix_getch(void);
int main(int argc, char** argv);

#endif


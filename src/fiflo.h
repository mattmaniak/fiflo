#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint16_t term_t; // Unsigned short as in "sys/ioctl.h".
typedef int16_t buff_t; // Only for amount indicators.

#define MEMBLOCK 4096 // For optimal mallocation.
#define MAX_LNS (buff_t) pow(2, (sizeof(buff_t) * 8))
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

void ignore_sig(int nothing);
void argc_check(int arg_count);
void options(const char* arg);
void run(const char* passed);
int main(int argc, char** argv);

#endif


#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint16_t term_t;
typedef int32_t buff_t;

#define MAX_LNS (buff_t) powf(2, (sizeof(buff_t) * 8))
#define MAX_CHRS MAX_LNS - 1 // 1 for NULL

#define BLOCK_SZ 4096

typedef struct
{
	char* fname; // Full filename, eg. /home/user/basename
	char** txt; // Eg. txt[lns][chrs].
	buff_t chrs;
	buff_t lns;
	buff_t chrs_ln; // Chars in the line.
}
buff;

void ignore_sig(int nothing);
void run(char* name);
void argc_check(int arg_count);
int main(int argc, char** argv);

#endif


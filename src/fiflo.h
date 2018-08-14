#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint16_t term_t;
typedef int8_t buff_t; // TEMPONARY.

#define MAX_LNS (buff_t) powf(2, (sizeof(buff_t) * 8))
#define MAX_CHRS MAX_LNS - 1 // 1 for NULL. TODO

typedef struct
{
	char* fname; // Full filename, eg. /home/user/basename
	char** txt;
	buff_t chrs;
	buff_t chrs_ln; // Chars in the current line.
	buff_t ln;
}
buff;

void ignore_sig(int nothing);
void run(char* name);
void argc_check(int arg_count);
int main(int argc, char** argv);

#endif


#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint16_t term_t;
typedef int32_t buff_t;

#define MAX_CHARS MAX_LINES - 1 // 1 for NULL.
#define MAX_LINES (buff_t) powf(2, (sizeof(buff_t) * 8))

typedef struct
{
	char* fname; // Full filename, eg. /home/user/basename
	char* text; // Malloc'ed pointer with all typed/read chars.
	buff_t chars; // Amount
	buff_t lines; // indicators.
}
buff;

void ignore_sig(int nothing);
void run(char* name);
void argc_check(int arg_count);
int main(int argc, char* argv[]);

#endif


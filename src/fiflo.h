#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint16_t term_t;
typedef int32_t buff_t;

#define MAX_CHARS (buff_t) powf(2, (sizeof(buff_t) * 8)) - 1// 1 for NULL.
#define MAX_LINES MAX_CHARS + 1

typedef struct
{
	char* filename; // Full, eg. /home/user/basename
	char* text; // Malloc'ed pointer with all typed/read chars.
	buff_t chars; // Amount
	buff_t lines; // indicators.
}
buff;

void run(char* name);
void ignore_sig(int nothing);
void argc_check(int arg_count);
int main(int argc, char* argv[]);

#endif


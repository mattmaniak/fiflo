#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_CHARS_PER_LINE 80

typedef uint16_t term_t;
typedef int32_t buff_t;

#define MAX_CHARS (int) powf(2, (sizeof(buff_t) * 8)) - 1

typedef struct
{
	char *filename; // Full, eg. /home/user/basename
	char *text; // Malloc'ed pointer with all typed/read chars.
	buff_t chars; // Amounts
	buff_t lines; // indicators.
}
data;

void run(char *name);
void ignore_sig(int nothing);
void argc_check(int arg_count);
int main(int argc, char *argv[]);

#endif


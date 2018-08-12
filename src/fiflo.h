#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint16_t term_t;
typedef int8_t buff_t;

#define MAX_CHARS MAX_LINES - 1 // 1 for NULL.
#define MAX_LINES 10000 // (buff_t) powf(2, (sizeof(buff_t) * 8))

typedef struct
{
	char* fname; // Full filename, eg. /home/user/basename
	char* row[MAX_LINES];
	// TODO: POINTER TO LINE.
	buff_t chars; // Amount
	buff_t lines; // indicators.
}
buff;

void ignore_sig(int nothing);
void run(char* name);
void argc_check(int arg_count);
int main(int argc, char* argv[]);

#endif


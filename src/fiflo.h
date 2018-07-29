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

struct Data // There is no typedef to provide better code readability.
{
	char *filename; // Eg. /home/user/basename
	char *text;
	buff_t chars;
	buff_t lines;
};

void run(char *name);
void ignoreSig(int nothing);
void argcChceck(void);
int main(int argc, char *argv[]);

#endif


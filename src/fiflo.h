#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

#define BUFF_SZ SHRT_MAX - 1 // Always [TYPE]_MAX - 1.
typedef int16_t buff_t;

struct Params
{
	char *filename; // Eg. /home/user/basename.asdf
	buff_t chars;
	buff_t lines;
	char text[BUFF_SZ];
};

void showHelp(void);
void programRound(char *name);
//void freeExit(int nothing);
void argcChceck(void);

#endif


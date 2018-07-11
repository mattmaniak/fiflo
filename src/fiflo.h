#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

#define BUFF_SZ SHRT_MAX - 1 // Always [TYPE]_MAX - 1.
typedef int16_t BUFF_T;

struct Params
{
	char *filename;
	BUFF_T chars;
	BUFF_T lines;
	char text[BUFF_SZ];
};

void sigHandler(int nothing);
void showHelp(void);
void programRound(char *name);
void argcChceck(void);

#endif


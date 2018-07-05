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
	BUFF_T chars;
	BUFF_T lines;
	BUFF_T cursor_pos;
	char text[BUFF_SZ];
};

void sigHandler(int std_holder);
void usageInfo(void);
void argcChceck(void);
void programRound(void);

#endif


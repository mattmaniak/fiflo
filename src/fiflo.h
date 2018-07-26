#ifndef FIFLO_H
#define FIFLO_H

#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

#define BUFF_SZ SHRT_MAX - 1
typedef int16_t buff_t; // > 16 makes stack error.

struct Data // There is no typedef to provide better code readibility.
{
	char *filename; // Eg. /home/user/basename.asdf
	char *text;
	buff_t chars;
	buff_t lines;
};

void showHelp(void);
void showVersion(void);
void run(char *name);
void argcChceck(void);
int main(int argc, char *argv[]);

#endif


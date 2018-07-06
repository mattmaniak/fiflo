#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

// ANSI escape codes.
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define BLINK "\033[5m"
#define INVERT "\033[7m"

#define X 1
#define Y 0

#define BARS_AMOUNT 3

typedef int16_t BUFF_T;
typedef uint16_t WIN_SIZE; // TODO: MERGE THESE TYPEDEF WITH LOGIC_H.

void cursor(void);
BUFF_T decIntLen(BUFF_T chars);
WIN_SIZE getSize(bool axis);
void upperBar(const char *basename);
WIN_SIZE autoFill(WIN_SIZE fill, char key, struct Params buff);
void lowerBar(char key, struct Params buff);
void cleanFrame(void);

#endif


#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI escape codes.
#define RESET "\033[0m\0"
#define BOLD "\033[1m\0"
#define BLINK "\033[5m\0"

void cursor(void);
static int8_t decimalIntLen(int8_t chars);
void infoBar(int8_t lines, int8_t chars, char base_filename[]);
void help(void);

#endif


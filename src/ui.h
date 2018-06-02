#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <string.h>

// ANSI escape codes.
#define RESET "\033[0m\0"
#define BOLD "\033[1m\0"
#define BLINK "\033[5m\0"

void cursor(void);
int8_t decimalIntLen(int8_t chars);
void infoBar(char filename[32], int8_t chars, int8_t lines);

#endif


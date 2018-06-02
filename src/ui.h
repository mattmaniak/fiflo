#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <string.h>

// ANSI escape codes.
#define RESTORE "\033[0m\0"
#define BOLD "\033[1m\0"

int8_t decimalIntLen(int8_t chars);
void infoBar(char filename[32], int8_t chars, int8_t lines);

#endif


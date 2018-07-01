#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI escape codes.
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define BLINK "\033[5m"
#define INVERT "\033[7m"

#define X 1
#define Y 0

typedef int16_t BUFF_T;
typedef uint16_t WIN_DIMENSION; // TODO: MERGE THESE TYPEDEF WITH LOGIC_H.

int8_t decIntLen(int8_t chars);
WIN_DIMENSION termSize(bool axis);
void upperBar(const char *basename);
WIN_DIMENSION autoFill(WIN_DIMENSION fill, char key);
void lowerBar(BUFF_T lines, BUFF_T chars, char key);
void cleanFrame(void);

#endif


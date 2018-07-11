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

#define X true
#define Y false

#define BARS_SZ 3

typedef int16_t buff_t;
typedef uint16_t term_t; // TODO: MERGE THESE TYPEDEF WITH LOGIC_H.

void cursor(void);
buff_t decIntLen(buff_t chars);
term_t getSize(bool axis);
void printDynamicString(const char *string, term_t max_len);
void upperBar(const char *fname);
term_t autoFill(struct Params buff, char key, term_t fill);
void lowerBar(struct Params buff, char key);
void cleanFrame(void);

#endif


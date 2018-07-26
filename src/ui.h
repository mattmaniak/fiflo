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

#define BAR_SZ 2

#define MIN_WIDTH 80
#define MIN_HEIGHT 10
#define MAX_WIDTH 65534
#define MAX_HEIGHT 65534

typedef int16_t buff_t;
typedef uint16_t term_t;

buff_t decIntLen(buff_t chars);
term_t getSize(bool axis);
void linesLimit(buff_t lines);
void printDynamicFilename(const char *string, const char *prog, term_t max_len);
void bar(struct Data buff, char key);
void cleanFrame(void);

#endif


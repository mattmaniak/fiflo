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
#define INVERT "\033[7m"

#define BAR_SZ 2

#define MIN_WIDTH 80
#define MIN_HEIGHT 10
#define MAX_WIDTH 65534
#define MAX_HEIGHT 65534

#define MAX_CHARS 0x7fffffff - 1

typedef int32_t buff_t;
typedef uint16_t term_t;

buff_t decIntLen(buff_t chars);
void printDynamicFilename(const char *string, const char *prog, term_t max_len);
void bar(struct Data buff, char key);

#endif


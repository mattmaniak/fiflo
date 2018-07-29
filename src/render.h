#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define READ 1
#define WRITE 0

#define X true
#define Y false

#define BAR_SZ 2

#define MIN_X 80
#define MIN_Y 10
#define MAX_X 65534 // USHRT_MAX - 1
#define MAX_Y 65534

// ANSI escape codes.
#define RESET "\033[0m"
#define INVERT "\033[7m"

void showHelp(void);
void showVersion(void);
void printDynamicFilename(const char *string, const char *prog, term_t max_len);
void bar(struct Data buff, char key);
term_t termSize(bool axis);
void cleanFrame(void);
void renderText(struct Data buff);
void windowFill(buff_t lines);
struct Data window(struct Data buff, char key);

#endif


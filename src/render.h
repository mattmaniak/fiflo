#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define X true
#define Y false

#define BAR_SZ 2

#define MIN_X 80 // MIN is before name to tell apart from limits.h macros.
#define MIN_Y 10
#define MAX_X USHRT_MAX - 1 // 65534
#define MAX_Y USHRT_MAX - 1

// ANSI escape codes.
#define RESET "\033[0m"
#define INVERT "\033[7m"

void help(void);
void version(void);

term_t get_term_sz(bool axis);
void flush_window(void);

void print_fname(const char *string, const char *prog, term_t max_len);
void bar(data buff, char key);
void print_text(data buff);
data window(data buff, char key);
void fill_window(buff_t lines);

#endif


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
#define CURSOR_UP "\033[A"
#define CURSOR_DOWN "\033[B"
#define CURSOR_RIGHT "\033[C"
#define CURSOR_LEFT "\033[D"
#define GO_UPPER_LINE "\033[F"
#define CLEAN_LINE "\033[K"

void help(void);
void version(void);

term_t get_term_sz(bool axis);
void flush_window(buff_t lines);

void print_fname(const char *string, const char *prog, term_t max_len);
void bar(buff data, char key);
void print_text(buff data);
buff window(buff data, char key);

#endif


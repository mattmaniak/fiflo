#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define BAR_SZ 2
#define PLACE_FOR_TEXT (get_term_sz('Y') - BAR_SZ)

#define MIN_X 80 // MIN/MAX are before names to tell apart from limits.h macros.
#define MIN_Y 10
#define MAX_X USHRT_MAX - 1 // 65534
#define MAX_Y MAX_X

// ANSI escape codes.
#define RESET "\033[0m"
#define INVERT "\033[7m"
#define CURSOR_UP "\033[A"
#define CURSOR_DOWN "\033[B"
#define CURSOR_RIGHT "\033[C"
#define CURSOR_LEFT "\033[D"
#define LINE_UP "\033[F"
#define CLEAN_WHOLE_LINE "\033[2K"

// Every seen thing.
void help(void);
void version(void);

term_t get_term_sz(char axis);
void flush_window(buff_t lines); // For rendering in a one frame.

void print_fname(const char *prog, char *fname, term_t max_len); // Dynamic.
void bar(buff data, char key); // Render the only bar: upper. Contains info.

buff_t scroll(buff data); // Ignores first chars to leave a place for the rest.
void set_cursor_pos(buff data);
void print_text(buff data); // Pressed keys to rendered chars in proper order.
void window(buff data, char key); // Bar + rendered text + fill + cursor.

#endif


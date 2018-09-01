#ifndef RENDER_H
#define RENDER_H // Every seen thing.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define BAR_SZ 2
#define CURRENT 1
#define TXT_AREA (get_term_sz('Y') - BAR_SZ)

// ANSI escape codes..
#define RESET "\033[0m"
#define INVERT "\033[7m"
#define LINE_UP "\033[F"
#define CLEAN_LINE "\033[2K"
#define CURSOR_UP(n) printf("\033[%dA", n)
#define CURSOR_DOWN(n) printf("\033[%dB", n)
#define CURSOR_RIGHT(n) printf("\033[%dC", n)
#define CURSOR_LEFT(n) printf("\033[%dD", n)

#define DOTS "[...]"

// Shown after certain program's parameter.
void help(void);
void version(void);

term_t get_term_sz(char axis);
void flush_window(buff dt); // For rendering in a one frame.
void draw_bar(buff dt, char key);

void lower_fill(buff_t lns);
void set_cursor_pos(buff dt);
void window(buff dt, char key); // Bar + rendered text + fill + cursor.

#endif


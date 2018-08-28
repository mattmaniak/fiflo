#ifndef RENDER_H
#define RENDER_H // Every seen thing.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define BAR_SZ 2
#define CURRENT_LN 1
#define TXT_AREA (get_term_sz('Y') - BAR_SZ)

// ANSI escape codes.
#define COLORS_RESET "\033[0m"
#define COLORS_INVERT "\033[7m"
#define MV_CURSOR_UP(n) printf("\033[%dA", n)
#define MV_CURSOR_DOWN(n) printf("\033[%dB", n)
#define MV_CURSOR_RIGHT(n) printf("\033[%dC", n)
#define MV_CURSOR_LEFT(n) printf("\033[%dD", n)
#define LINE_UP "\033[F"
#define CLEAN_WHOLE_LINE "\033[2K"

// Shown after certain program's parameter.
void help(void);
void version(void);

term_t get_term_sz(char axis);
void flush_window(buff dat); // For rendering in a one frame.
void draw_bar(buff dat, char key);

void lower_fill(buff_t lns);
void set_cursor_pos(buff dat);
void window(buff dat, char key); // Bar + rendered text + fill + cursor.

#endif


#ifndef RENDER_H
#define RENDER_H // Every seen thing.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define BAR_SZ 2
#define CURRENT 1
#define TXT_AREA (termgetsz(dt, 'Y') - BAR_SZ)
#define TERM_X_MIN 46

// ANSI escape codes..
#define RESET "\033[0m"
#define INVERT "\033[7m"
#define LINE_UP "\033[F"
#define CLEAN_LN "\033[2K"
#define CURSOR_UP(n) printf("\033[%dA", n)
#define CURSOR_DOWN(n) printf("\033[%dB", n)
#define CURSOR_RIGHT(n) printf("\033[%dC", n)
#define CURSOR_LEFT(n) printf("\033[%dD", n)

#define SAVE_CURSOR_POS() printf("%s", "\033[s");
#define RESTORE_CURSOR_POS() printf("%s", "\033[u");

#define INDEX 1

#define DOTS "[...]"

term_t termgetsz(buf dt, char axis); // Check if a term to small or big.
void flushwin(buf dt); // For rendering in a one frame.

void bar(buf dt, char key); // Render upper bar.
void window(buf dt, char key); // Bar + rendered text + fill + cursor.
void lower_fill(buf dt); // Empty space below the text.
void setcurspos(buf dt); // Set cursor position from the rendered bottom.

#endif


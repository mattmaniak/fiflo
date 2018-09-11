#ifndef RENDER_H
#define RENDER_H // Every seen thing.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define BAR_SZ 2
#define TXT_AREA (termgetsz(dt, 'Y') - BAR_SZ)
#define TERM_X_MIN 68
#define DOTS "[...]"

// ANSI escape codes..
#define RESET "\033[0m"
#define INVERT "\033[7m"
#define LINE_UP "\033[F"
#define CLEANLN "\033[2K"
#define CURSUP(n) printf("\033[%dA", n)
#define CURSDOWN(n) printf("\033[%dB", n)
#define CURSRIGHT(n) printf("\033[%dC", n)
#define CURSLEFT(n) printf("\033[%dD", n)
#define SAVECURSPOS() printf("%s", "\033[s");
#define RESTORECURSPOS() printf("%s", "\033[u");

term_t termgetsz(buf dt, char axis); // Check if a term to small or big.
void flushwin(buf dt); // For rendering in a one frame.

void bar(buf dt, char key); // Render upper bar.
void window(buf dt, char key); // Bar + rendered text + fill + cursor.
void lower_fill(buf dt); // Empty space below the text.
void setcurspos(buf dt); // Set cursor position from the rendered bottom.

#endif


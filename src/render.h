#ifndef RENDER_H
#define RENDER_H // Every seen thing.

// Common rest is included in 'fiflo.h' There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define BAR_SZ 2
#define TXT_Y (termgetsz(dt, 'Y') - BAR_SZ)
#define TERM_X_MIN 68
#define DOTS "[...]"

// ANSI escape codes..
#define RESET "\e[0m"
#define INVERT "\e[7m"
#define LINE_UP "\e[F"
#define CLEANLN "\e[2K"
#define CURSUP(n) printf("\e[%dA", n)
#define CURSDOWN(n) printf("\e[%dB", n)
#define CURSRIGHT(n) printf("\e[%dC", n)
#define CURSLEFT(n) printf("\e[%dD", n)
#define SAVECURSPOS() printf("%s", "\e[s");
#define RESTORECURSPOS() printf("%s", "\e[u");

term_t termgetsz(meta* dt, char axis); // Check if a term to small or big.
void flushwin(meta* dt); // For rendering in a one frame.

void bar(meta* dt, char key); // Render upper bar.
void window(meta* dt, char key); // Bar + rendered text + fill + cursor.
void lower_fill(meta* dt); // Empty space below the text.
void setcurspos(meta* dt); // Set cursor position from the rendered bottom.

#endif


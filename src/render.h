#ifndef RENDER_H
#define RENDER_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define AT_LEAST_CHAR 1 // Needed to printf's wiDath specifier wnich is >= 1.
#define SPACE_SZ      1
#define CUR_SZ        1
#define UPBAR_SZ      1
#define SLASH_SZ      1
#define LBAR_SZ       1
#define BARS_SZ       (UPBAR_SZ + LBAR_SZ)
#define LBAR_STR      "CTRL+: C - exit/ Z - minimize/ D - save\
YGHB - move cursor\0"
#define TERM_X_MIN    (term_t) (strlen(LBAR_STR) + AT_LEAST_CHAR)

#define TXT_X (termgetsz('X', Dat) - STRLEN_BUF_T)
#define TXT_Y (termgetsz('Y', Dat) - BARS_SZ)

// ANSI escape codes. TODO: NAMES.
#define A_RESET()           printf("%s", "\033[0m")
#define A_INVERT_COLORS()   printf("%s", "\033[7m")
#define A_CLEAN_LN()        printf("%s", "\033[2K")
#define A_CUR_UP(n)         printf("\033[%dA", n)
#define A_CUR_DOWN(n)       printf("\033[%dB", n)
#define A_CUR_RIGHT(n)      printf("\033[%dC", n)
#define A_CUR_LEFT(n)       printf("\033[%dD", n)
#define A_SAVE_CUR_POS()    printf("%s", "\033[s")
#define A_RESTORE_CUR_POS() printf("%s", "\033[u")

#define LF 10

term_t termgetsz(char axis, meta* Dat); // Check if a term to small or big.
void flushwin(meta* Dat);               // Clean the old rendered window.

void ubar(meta* Dat);        // Render the upper bar.
void xscrolltxt(buf_t ln, meta* Dat);
buf_t yscrolltxt(meta* Dat);
void rendertxt(meta* Dat);   // And care about Dat->txt scrolling.
void fill(meta* Dat);        // Empty space below the text.
void lbar(meta* Dat);        // Render the lower bar that contains keyboard info.
void window(meta* Dat);      // Fills the whole visible terminal area.
void setcurpos(meta* Dat);   // Set cursor position from the rendered bottom.
#endif


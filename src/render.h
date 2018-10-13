#ifndef RENDER_H
#define RENDER_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define AT_LEAST_CHAR 1
#define SPACE_SZ      1
#define CUR_SZ        1
#define SLASH_SZ      1
#define UPBAR_SZ      1
#define LBAR_SZ       1
#define BARS_SZ       (UPBAR_SZ + LBAR_SZ)
#define LBAR_STR      "CTRL+: X - exit/ D - save/ YGHB - move cursor\0"
#define TERM_X_MIN    (term_t) (strlen(LBAR_STR) + AT_LEAST_CHAR)

#define TXT_X (termgetsz('X', Dt) - STRLEN_BUF_T)
#define TXT_Y (termgetsz('Y', Dt) - BARS_SZ)


#define ANSI_RESET()           printf("\033[%s", "0m")
#define ANSI_BOLD()            printf("\033[%s", "1m")
#define ANSI_INVERT()          printf("\033[%s", "7m")
#define ANSI_CLEAN_LN()        printf("\033[%s", "2K")
#define ANSI_CUR_UP(n)         printf("\033[%dA", n)
#define ANSI_CUR_DOWN(n)       printf("\033[%dB", n)
#define ANSI_CUR_RIGHT(n)      printf("\033[%dC", n)
#define ANSI_CUR_LEFT(n)       printf("\033[%dD", n)
#define ANSI_SAVE_CUR_POS()    printf("\033[%s", "s")
#define ANSI_RESTORE_CUR_POS() printf("\033[%s", "u")

#define LF 10

term_t termgetsz(char axis, meta* Dt); // Check if a term to small or big.
void flushwin(meta* Dt);               // Clean the old rendered window.

void ubar(meta* Dt);      // Render the upper bar.
void xscrolltxt(buf_t ln, meta* Dt);
buf_t yscrolltxt(meta* Dt);
void numln(buf_t ln);
void rendertxt(meta* Dt); // And care about Dt->txt scrolling.
void fill(meta* Dt);      // Empty space below the text.
void lbar(void);          // Render the lower bar that contains keyboard info.
void window(meta* Dt);    // Fills the whole visible terminal area.
void setcurpos(meta* Dt); // Set cursor position from the rendered bottom.
#endif


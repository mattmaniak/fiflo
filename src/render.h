#ifndef RENDER_H
#define RENDER_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define BARS_SZ    2
#define UPBAR_SZ   1
#define LBAR_SZ    1
#define TERM_X_MIN 68
#define CUR_SZ     1

#define TXT_X (term_sz(dt, 'x') - STRLEN_BUF_T)
#define TXT_Y (term_sz(dt, 'y') - BARS_SZ)

// ANSI escape codes. TODO: NAMES.
#define ANSI_RESET()           printf("%s", "\033[0m")
#define ANSI_INVERT()          printf("%s", "\033[7m")
#define LINE_UP()         printf("%s", "\033[F")
#define CLEANLN()         printf("%s", "\033[2K")
#define MV_CUR_UP(n)      printf("\033[%dA", n)
#define MV_CUR_DOWN(n)    printf("\033[%dB", n)
#define MV_CUR_RIGHT(n)   printf("\033[%dC", n)
#define MV_CUR_LEFT(n)    printf("\033[%dD", n)
#define SAVE_CUR_POS()    printf("%s", "\033[s")
#define RESTORE_CUR_POS() printf("%s", "\033[u")

term_t term_sz(meta* dt, char axis); // Check if a term to small or big.
void flush_win(meta* dt); // For rendering in a one frame.

void upper_bar(meta* dt); // Render upper bar.
void lower_bar(meta* dt, char key);
void window(meta* dt, char key); // Bar + rendered text + fill + cursor.
void fill(meta* dt); // Empty space below the text.
void set_cursor_pos(meta* dt); // Set cursor position from the rendered bottom.
#endif


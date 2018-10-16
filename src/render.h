#ifndef RENDER_H
#define RENDER_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas.
#define AT_LEAST_CHAR 1
#define CUR_SZ        1
#define SLASH_SZ      1
#define UPBAR_SZ      1
#define LBAR_SZ       1
#define BARS_SZ       (UPBAR_SZ + LBAR_SZ)
#define LBAR_STR      "CTRL+: X - exit/ D - save/ YGHB - move cursor\0"
#define TERM_X_MIN    (term_t) (strlen(LBAR_STR) + AT_LEAST_CHAR)

#define TXT_X (get_term_sz('X', Dt) - STRLEN_BUF_T)
#define TXT_Y (get_term_sz('Y', Dt) - BARS_SZ)

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

// Returns current terminal width and height and exits if is wrong.
term_t get_term_sz(char axis, meta* Dt);

// Clean the whole rendered window.
void flush_window(meta* Dt);

// Renders the upper bar with a filename and indicators.
void upper_bar(meta* Dt);

// Scrolls chars. Used when the cursor is in static position.
void scroll_chrs_in_ln(buf_t ln, meta* Dt);

// Returns value of hidden lines.
buf_t scroll_lns(meta* Dt);

// Prints a line number.
void print_ln_num(buf_t ln);

// Show the text in the window.
void display_txt(meta* Dt);

// Vertical fill between the text and lower bar. If there isn't many lines.
void fill(meta* Dt);

// Renders the lower bar that contains keyboard info.
void lower_bar(void);

// Stupid wrapper for above things.
void window(meta* Dt);

// Sets the cursor position from the left bottom to a last char.
void set_cur_pos(meta* Dt);
#endif


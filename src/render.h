#ifndef RENDER_H
#define RENDER_H

// Common rest is included in "fiflo.h". There are only file-specific imports.
#include <sys/ioctl.h>

// UI areas:
// Some semantic macros.
#define CUR_SZ   1
#define SLASH_SZ 1

#define UBAR_SZ  2
#define LBAR_SZ  1 // Must be equal 1 otherwise will break rendering a little.
#define BARS_SZ  (UBAR_SZ + LBAR_SZ)

// Text that is shown on the lower bar. Also defines the minimal terminal width.
#define LBAR_STR \
"CTRL^: X - exit; W - save; G/H - cursor; S/Q - render/hide new chars\0"

// Remember to not override the upper bar width.
#define TERM_X_MIN (term_t) strlen(LBAR_STR)

// Sizes of the text area.
#define TXT_X (term_t) (get_term_sz('X', Buff) - STRLEN_BUF_T)
#define TXT_Y (term_t) (get_term_sz('Y', Buff) - BARS_SZ)

// ANSI escape codes:
#define ANSI_RESET()           printf("\033[%s", "0m")

// If you want to change to inverted bars' colors, set "1m" to "7m".
#define ANSI_BOLD()            printf("\033[%s", "1m")
#define ANSI_CLEAN_LN()        printf("\033[%s", "2K")
#define ANSI_CUR_UP(offset)    printf("\033[%dA", offset)
#define ANSI_CUR_DOWN(offset)  printf("\033[%dB", offset)
#define ANSI_CUR_RIGHT(offset) printf("\033[%dC", offset)
#define ANSI_CUR_LEFT(offset)  printf("\033[%dD", offset)
#define ANSI_SAVE_CUR_POS()    printf("\033[%s", "s")
#define ANSI_RESTORE_CUR_POS() printf("\033[%s", "u")

// Returns current terminal width and height and exits if is wrong.
term_t get_term_sz(char axis, f_mtdt* Buff);

// Clean the whole rendered window.
void flush_window(f_mtdt* Buff);

// Renders the upper bar with a filename and indicators.
void upper_bar(f_mtdt* Buff);

// Scrolls chars. Used when the cursor is in static position.
void scroll_line_x(f_mtdt* Buff);

// Returns value of hidden lines.
buff_t scroll_lines(f_mtdt* Buff);

// Prints a line number.
void print_line_num(buff_t line);

// Shows a text in the window.
void display_text(f_mtdt* Buff);

// Vertical fill between the text and lower bar. If there isn't many lines.
void fill(f_mtdt* Buff);

// Renders the lower bar that contains keyboard info.
void lower_bar(void);

// Stupid wrapper for above things.
void window(f_mtdt* Buff);

// Sets the cursor position from the left bottom.
void set_cur_pos(f_mtdt* Buff);

#endif


#ifndef UI_H
#define UI_H

// Data about a current viewport.

#include <stdio.h>

// Some semantic macros.
#define CUR_SZ   1
#define LF_SZ    1
#define SLASH_SZ 1
#define SPACE_SZ 1

#define ICON     "|`` " // The upper and lower parts are the same, so it's half.
#define ICON_LEN 4

#define UBAR_SZ 2
#define LBAR_SZ 1 // Must be equal 1, otherwise will break rendering a little.

#define TOGGLED_PANE_SZ 5

// Text that is shown on the lower bar. Also defines the minimal terminal width.
#define LBAR_STR "CTRL^\\ - key bindings pane"

#define WRAP_LINE() putchar('\n')

// ANSI escape codes.
#define ANSI_RESET()           printf("\x1b[%s", "0m")
#define ANSI_INVERT()          printf("\x1b[%s", "7m")
#define ANSI_CLEAN_LINE()      printf("\x1b[%s", "2K")
#define ANSI_CUR_UP(offset)    printf("\x1b[%uA", offset)
#define ANSI_CUR_DOWN(offset)  printf("\x1b[%uB", offset)
#define ANSI_CUR_RIGHT(offset) printf("\x1b[%uC", offset)
#define ANSI_CUR_LEFT(offset)  printf("\x1b[%uD", offset)
#define ANSI_SAVE_CUR_POS()    printf("\x1b[%s", "s")
#define ANSI_RESTORE_CUR_POS() printf("\x1b[%s", "u")

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

#pragma pack(push, 1)
typedef struct
{
	char   line_num_str[16]; // Place for string of the highest line number.
	term_t line_num_len;     // Dynamic width of the lines numbers.
	term_t text_x;           // Horizontal space for the text (width: chars).
	term_t text_y;           // Vertical space for the text (lines).
	term_t lbar_h;           // Lower bar height (lines).
	term_t pane_h;           // As above but toggled.
}
win_mtdt;
#pragma pack(pop)

#endif
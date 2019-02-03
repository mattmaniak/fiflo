#ifndef UI_H
#define UI_H

// User interface components.

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

// Some semantic macros.
#define GIT_LOGO        "|\\ git "
#define GIT_LOGO_LENGTH 7

#define DOTS_LENGTH 3

#define UBAR_SZ 2
#define LBAR_SZ 1 // Must be equal 1, otherwise will break rendering a little.

#define TOGGLED_PANE_SZ 5

#define WRAP_LINE() putchar('\n')

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

typedef struct
{
	char   line_num_str[16]; // Place for string of the highest line number.
	term_t line_num_len;     // Dynamic width of the lines numbers.
	term_t text_x;           // Horizontal space for the text (width: chars).
	term_t text_y;           // Vertical space for the text (lines).
	term_t lbar_h;           // Lower bar height (lines).
	term_t pane_h;           // As above but toggled.

	term_t win_w;
	term_t win_h;
}
Ui_t;

// Prints the line number.
void ui__print_line_number(Buff_t*, Conf_t*, idx_t, term_t);

// Renders the upper bar with a filename and indicators.
void ui__upper_bar(Buff_t*, Conf_t*, Ui_t*);

// Renders the lower bar that contains keyboard info.
void ui__lower_bar(Buff_t*, Conf_t*, Ui_t*);

#endif

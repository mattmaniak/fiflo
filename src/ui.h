#ifndef UI_H
#define UI_H

// User interface components.

#include <string.h>

#include "buffer.h"
#include "config.h"
#include "modes.h"

#define GIT_LOGO            "git |\\ "
#define GIT_LOGO_LENGTH     (int) strlen(GIT_LOGO)
#define GIT_BRANCH_AREA_MIN 10

#define UBAR_SZ 2
#define LBAR_SZ 1 // Must be equal 1, otherwise will break rendering a little.

#define TOGGLED_PANE_SZ 5
#define LEFT_PADDING  1
#define RIGHT_PADDING 1
#define HORIZONTAL_PADDING (LEFT_PADDING + RIGHT_PADDING)

#define WRAP_LINE() putchar('\n') // Not the text. Needed to rendering.

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

typedef struct
{
    term_t line_num_length; // Dynamic width of the lines numbers.
    term_t text_x;          // Horizontal space for the text (width: chars).
    term_t text_y;          // Vertical space for the text (lines).
    term_t lbar_h;          // Lower bar height (lines).
    term_t pane_h;          // As above but toggled.
    term_t win_w;
    term_t win_h;
}
Ui_t;

// Used to style the UI.
void ui__set_color(const Opt_t* const);

// Prints the line number.
void ui__print_line_number(const Buff_t* const, const Conf_t* const,
                           const idx_t, const term_t);

// Renders the upper bar with a filename and indicators.
void ui__upper_bar(const Buff_t* const, const Conf_t* const, const Ui_t* const);

// Renders the lower bar that contains keyboard info.
void ui__lower_bar(const Buff_t* const, const Conf_t* const, const Mod_t* const,
                   const Ui_t* const);

#endif

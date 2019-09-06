#ifndef UI_H
#define UI_H

// User interface components.

#include <string.h>

#include "buffer.h"
#include "config.h"
#include "modes.h"

#define UI__GIT_LOGO         "git |\\ "
#define UI__GIT_LOGO_W       (int) strlen(UI__GIT_LOGO)
#define UI__GIT_BRANCH_MIN_W 10

#define UI__UBAR_SZ 2
#define UI__LBAR_SZ 1 // Must be equal 1, otherwise will break rendering.

#define UI__TOGGLED_LBAR_H     6
#define UI__LEFT_PADDING       1
#define UI__RIGHT_PADDING      1
#define UI__HORIZONTAL_PADDING (UI__LEFT_PADDING + UI__RIGHT_PADDING)

#define WRAP_LINE() putchar('\n') // Not the text. Needed to rendering.

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

typedef struct
{
    term_t line_num_length; // Dynamic width of the lines numbers.
    term_t textarea_w;          // Horizontal space for the text (width: chars).
    term_t textarea_h;          // Vertical space for the text (lines).
    term_t lbar_h;          // Lower bar height (lines).
    term_t toggled_lbar_h;  // As above but toggled.
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
                   const Ui_t* const, const idx_t, const idx_t);

#endif

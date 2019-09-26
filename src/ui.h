#ifndef UI_H
#define UI_H

// User interface components.

#include <string.h>

#include "v_file.h"
#include "config.h"
#include "modes.h"

#define UI__GIT_LOGO         "git |\\ "
#define UI__GIT_LOGO_W       (int) strlen(UI__GIT_LOGO)
#define UI__GIT_BRANCH_MIN_W 10

#define UI__UBAR_SZ    2
#define UI__LBAR_SZ    1 // Must be equal 1, otherwise will break rendering.
#define UI__MAX_LBAR_H 6

#define UI__LEFT_PADDING       1
#define UI__RIGHT_PADDING      1
#define UI__HORIZONTAL_PADDING (UI__LEFT_PADDING + UI__RIGHT_PADDING)

#define UI__WRAP_LN() putchar('\n') // Not a text. Needed to rendering.

typedef uint16_t term_t; // Unsigned short as in the "sys/ioctl.h".

typedef struct
{
    term_t ln_num_len;      // Dynamic width of line numbers.
    term_t txtarea_w;       // Horizontal space for a text (width: chars).
    term_t txtarea_h;       // Vertical space for a text (lines).
    term_t lbar_h;          // Lower bar height (lines).
    term_t expanded_lbar_h; // As above but toggled.
    term_t win_w;
    term_t win_h;

    const int16_t _padding;

    int pcard_delta_x;
}
Ui_t;

// Used to style a UI.
void ui__colorize(const int);

// Prints a line number.
void ui__print_line_number(const V_file_t* const, const Conf_t* const,
                           const size_t, const term_t);

// Renders the upper bar with a filename and indicators.
void ui__upper_bar(const V_file_t* const, const Conf_t* const,
                   const Ui_t* const);

// Renders the lower bar that contains keyboard info.
void ui__lower_bar(const V_file_t* const, const Conf_t* const,
                   const Mod_t* const, const Ui_t* const, const size_t,
                   const size_t);

#endif

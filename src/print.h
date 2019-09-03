#ifndef PRINT_H
#define PRINT_H

// Renders the text itself. Cares about scrolling.

#include <string.h>

#include "buffer.h"
#include "config.h"
#include "modes.h"
#include "ui.h"

#define PRINT__SPACE_HIGHLIGHT '.'
#define PRINT__TAB_HIGHLIGHT   '>'

// Converts stupidly-implemented tabs into some renderable spaces.
void print__line_with_tabs(const Buff_t* const, const Conf_t* const,
                           const idx_t, const idx_t, const idx_t);

// Returns value of hidden lines.
idx_t print__set_start_line(const Buff_t* const, const Ui_t* const);

// Decides how to show it. Can scroll it or the cursor.
void print__actual_line(const Buff_t* const, const Conf_t* const,
                        const Ui_t* const);

// Prints and shrinks line that is non-actual.
void print__another_line(const Buff_t* const, const Ui_t* const,
                         const Conf_t* const, const idx_t);

// Scrolls chars. Used when the cursor is in static position.
void print__scroll_line_horizontally(const Buff_t* const, const Conf_t* const,
                                     const Ui_t* const);

// Renders a text when there is smaller amount of lines than the window height.
void print__fit_lines(const Buff_t* const, const Ui_t* const,
                      const Conf_t* const);

// If there is more lines but they are scrolled to the start.
void print__shrink_lines(const Buff_t* const, const Ui_t* const,
                         const Conf_t* const);

// Lines rendering starts at specified by user offset.
void print__scroll_lines(const Buff_t* const, const Ui_t* const,
                         const Conf_t* const);

// Shows a text in the window.
void print__display_text(const Buff_t* const, const Ui_t* const,
                         const Conf_t* const);

#endif

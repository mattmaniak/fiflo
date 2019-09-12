#ifndef PRINT_H
#define PRINT_H

// Renders the text itself. Cares about scrolling.

#include <string.h>

#include "buffer.h"
#include "config.h"
#include "syntax.h"
#include "modes.h"
#include "ui.h"

#define PRINT__SPACE_HIGHLIGHT '.'
#define PRINT__TAB_HIGHLIGHT   '>'

// Convert specifically implemented tabs into some renderable spaces.
void print__line_with_tabs(const Buff_t* const, const Conf_t* const,
                           const Syntax_t* const, const Ui_t* const,
                           const idx_t, const idx_t, const idx_t);

//
void print__punch_card(const Conf_t* const, const Ui_t* const, const idx_t);

// Return value of hidden lines.
idx_t print__set_start_line(const Buff_t* const, const Ui_t* const);

// Decide how to display a line. Can scroll it or the cursor.
void print__actual_line(const Buff_t* const, const Conf_t* const,
                        const Syntax_t* const, const Ui_t* const);

// Print and shrink a line that without the cursor.
void print__another_line(const Buff_t* const, const Conf_t* const,
                         const Syntax_t* const, const Ui_t* const,
                         const idx_t);

// Scroll chars. Used when the cursor is in a static right position.
void print__scroll_line_horizontally(const Buff_t* const, const Conf_t* const,
                                     const Syntax_t* const, const Ui_t* const);

// Render a text when there is smaller amount of lines than a window height.
void print__fit_lines(const Buff_t* const, const Conf_t* const,
                      const Syntax_t* const, const Ui_t* const);

/* Handle rendering if there is more lines but they are scrolled from the
   beginning. */
void print__shrink_lines(const Buff_t* const, const Conf_t* const,
                         const Syntax_t* const, const Ui_t* const);

// Render lines starting at a certain offset.
void print__scroll_lines(const Buff_t* const, const Conf_t* const,
                         const Syntax_t* const, const Ui_t* const);

// Show a text in a window.
void print__display_text(const Buff_t* const, const Conf_t* const,
                         const Syntax_t* const, const Ui_t* const);

#endif

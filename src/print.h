#ifndef PRINT_H
#define PRINT_H

// Renders the text itself. Cares about scrolling.

#include <string.h>

#include "v_file.h"
#include "config.h"
#include "syntax.h"
#include "modes.h"
#include "pcard.h"
#include "ui.h"

#define PRINT__SPACE_HIGHLIGHT '.'
#define PRINT__TAB_HIGHLIGHT   '>'

// Convert specifically implemented tabs into some renderable spaces.
void print__line_with_tabs(const V_file* const, const Config* const,
                           const Syntax* const, const size_t, const size_t,
                           const size_t);

// Return value of hidden lines.
size_t print__set_start_line(const V_file* const, const Ui* const);

// Decide how to display a line. Can scroll it or the cursor.
void print__actual_line(const V_file* const, const Config* const,
                        const Syntax* const, const Ui* const);

// Print and shrink a line that without the cursor.
void print__another_line(const V_file* const, const Config* const,
                         const Syntax* const, const Ui* const,
                         const size_t);

// Scroll chars. Used when the cursor is in a static right position.
void print__scroll_line_horizontally(const V_file* const,
                                     const Config* const,
                                     const Syntax* const, const Ui* const);

// Render a text when there is smaller amount of lines than a window height.
void print__fit_lines(const V_file* const, const Config* const,
                      const Syntax* const, const Ui* const);

/* Handle rendering if there is more lines but they are scrolled from the
   beginning. */
void print__shrink_lines(const V_file* const, const Config* const,
                         const Syntax* const, const Ui* const);

// Render lines starting at a certain offset.
void print__scroll_lines(const V_file* const, const Config* const,
                         const Syntax* const, const Ui* const);

// Show a text in a window.
void print__display_text(const V_file* const, const Config* const,
                         const Syntax* const, const Ui* const);

#endif

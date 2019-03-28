#ifndef PRINT_H
#define PRINT_H

// Renders the text itself. Cares about scrolling.

#define SPACE_HIGHLIGHT '-'
#define TAB_HIGHLIGHT   '>'

// Converts stupidly-implemented tabs into some renderable spaces.
void print__line_with_tabs(Buff_t*, Conf_t*, idx_t, idx_t, idx_t);

// Returns value of hidden lines.
idx_t print__set_start_line(Buff_t*, Ui_t*);

// Decides how to show it. Can scroll it or the cursor.
void print__actual_line(Buff_t*, Conf_t*, Ui_t*);

// Prints and shrinks line that is non-actual.
void print__another_line(Buff_t*, Ui_t*, Conf_t*, idx_t);

// Scrolls chars. Used when the cursor is in static position.
void print__scroll_line_horizontally(Buff_t*, Conf_t*, Ui_t*);

// Renders a text when there is smaller amount of lines than the window height.
void print__fit_lines(Buff_t*, Ui_t*, Conf_t*);

// If there is more lines but they are scrolled to the start.
void print__shrink_lines(Buff_t*, Ui_t*, Conf_t*);

// Lines rendering starts at specified by user offset.
void print__scroll_lines(Buff_t*, Ui_t*, Conf_t*);

// Shows a text in the window.
void print__display_text(Buff_t*, Ui_t*, Conf_t*);

#endif

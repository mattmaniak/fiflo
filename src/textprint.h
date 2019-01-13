#ifndef TEXTPRINT_H
#define TEXTPRINT_H

// Renders the text itself. Cares about scrolling.

#define BOLD_LINE_NUM true
#define THIN_LINE_NUM false

#define CURRENT_LINE true
#define ANOTHER_LINE false

// Prints and shrinks line that is non-actual.
void textprint_print_another_line(Buff_t* Buff, Ui_t* Ui, idx_t line_i);

// Returns value of hidden lines.
idx_t textprint_set_start_line(Buff_t* Buff, Ui_t* Ui);

// Scrolls chars. Used when the cursor is in static position.
void textprint_scroll_line_horizontally(Buff_t* Buff, Ui_t* Ui);

// Decides how to show it. Can scroll it or the cursor.
void textprint_print_actual_line(Buff_t* Buff, Ui_t* Ui);

// Renders a text when there is smaller amount of lines than the window height.
void textprint_fit_lines(Buff_t* Buff, Ui_t* Ui);

// If there is more lines but they are scrolled to the start.
void textprint_shrink_lines(Buff_t* Buff, Ui_t* Ui);

// Lines rendering starts at specified by user offset.
void textprint_scroll_lines(Buff_t* Buff, Ui_t* Ui);

// Shows a text in the window.
void textprint_display_text(Buff_t* Buff, Ui_t* Ui);

#endif

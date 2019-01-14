#ifndef WINDOW_H
#define WINDOW_H

// Contains all the elements that will be rendered.

#include <limits.h>
#include <sys/ioctl.h>
#include <unistd.h>

extern void textprint_display_text(Buff_t* Buff, Ui_t* Ui);

// Returns current terminal width and height and exits if is wrong.
term_t window_get_terminal_size(char axis);

// Clean the whole rendered window.
void window_flush(void);

// Renders the upper bar with a filename and indicators.
void window_upper_bar(Buff_t* Buff, Ui_t* Ui);

// Renders the lower bar that contains keyboard info.
void window_lower_bar(const bool pane_toggled);

// Vertical fill between the text and lower bar. If there isn't many lines.
void window_fill(Buff_t* Buff, Ui_t* Ui);

// Stupid wrapper for above things.
bool window_render(Buff_t* Buff);

// Sets the cursor position starting from the left bottom.
void window_set_cursor_position(Buff_t* Buff, Ui_t* Ui);

#endif

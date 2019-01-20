#ifndef WINDOW_H
#define WINDOW_H

// Contains all the elements that will be rendered.

#include <limits.h>
#include <sys/ioctl.h>
#include <unistd.h>

// ANSI escape codes for a terminal control.
#define ANSI_CLEAN_WHOLE_LINE()        printf("\x1b[2K")
#define ANSI_CURSOR_UP(offset)         printf("\x1b[%uA", offset)
#define ANSI_CURSOR_DOWN(offset)       printf("\x1b[%uB", offset)
#define ANSI_CURSOR_RIGHT(offset)      printf("\x1b[%uC", offset)
#define ANSI_CURSOR_LEFT(offset)       printf("\x1b[%uD", offset)
#define ANSI_SAVE_CURSOR_POSITION()    printf("\x1b[s")
#define ANSI_RESTORE_CURSOR_POSITION() printf("\x1b[u")

extern void print__display_text(Buff_t* Buff, Ui_t* Ui, Conf_t* Config);

// Returns current terminal width and height and exits if is wrong.
term_t window__get_terminal_size(char axis);

// Clean the whole rendered window.
void window__flush(void);

// Vertical fill between the text and lower bar. If there isn't many lines.
void window__fill(Buff_t* Buff, Ui_t* Ui);

// Stupid wrapper for above things.
bool window__render(Buff_t* Buff, Conf_t* Config);

// Sets the cursor position starting from the left bottom.
void window__set_cursor_position(Buff_t* Buff, Ui_t* Ui);

#endif

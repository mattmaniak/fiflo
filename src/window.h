#ifndef WINDOW_H
#define WINDOW_H

// Contains all the elements that will be rendered.

#include <sys/ioctl.h>

#include "buffer.h"
#include "config.h"
#include "modes.h"
#include "print.h"
#include "ui.h"

// ANSI escape codes for a terminal control.
#define ANSI_CLEAN_WHOLE_LINE()   printf("\033[2K")
#define ANSI_SAVE_CURSOR_POS()    printf("\033[s")
#define ANSI_RESTORE_CURSOR_POS() printf("\033[u")
#define ANSI_CURSOR_UP(offset)    printf("\033[%uA", offset)
#define ANSI_CURSOR_DOWN(offset)  printf("\033[%uB", offset)
#define ANSI_CURSOR_RIGHT(offset) printf("\033[%uC", offset)
#define ANSI_CURSOR_LEFT(offset)  printf("\033[%uD", offset)

// Returns current terminal width and height and exits if is wrong.
term_t window__get_terminal_sz(const char);

// Clean the whole rendered window.
void window__flush(void);

// Vertical fill between the text and lower bar. If there isn't many lines.
void window__fill(const Buff_t* const, const Ui_t* const);

// Sets the a cursor position starting from the left bottom.
void window__set_cursor_pos(const Buff_t* const, const Mod_t* const,
                            const Ui_t* const);

// Stupid wrapper for above things.
bool window__render(const Buff_t* const, const Conf_t* const,
                    const Mod_t* const);

#endif

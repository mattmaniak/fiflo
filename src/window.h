#ifndef WINDOW_H
#define WINDOW_H

// Contains all elements that will be rendered.

#include <sys/ioctl.h>

#include "ansi.h"
#include "v_file.h"
#include "config.h"
#include "modes.h"
#include "pcard.h"
#include "print.h"
#include "ui.h"

// Return current terminal width and height and exits if it's wrong.
term_t window__receive_term_sz(const char);

// Clean a whole rendered window.
void window__flush(void);

// Vertical fill between a text and lower bar. If there isn't many lines.
void window__fill(const V_file* const, const Config* const,
                  const Ui* const);

// Sets the cursor position starting from a left bottom.
void window__set_cursor_pos(const V_file* const, const Modes* const,
                            const Ui* const);

// Stupid wrapper for above things.
bool window__render(const V_file* const, const Config* const,
                    const Modes* const, const Syntax* const, const size_t,
                    const size_t);

#endif

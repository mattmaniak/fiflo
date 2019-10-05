#ifndef KEYS_H
#define KEYS_H

// Currently supported characters and their actions inside.

#include "ascii.h"
#include "v_file.h"
#include "config.h"
#include "modes.h"
#include "file_io.h"
#include "memory.h"
#include "edit.h"
#include "size.h"

extern bool input__printable_char(V_file* const, const char);

// Initialize a new line.
bool keys__linefeed(V_file* const);

// Remove a last char and optionally delete a last line.
bool keys__backspace(V_file* const, const Config* const, const Modes* const);

// Insert a specified amount of the '\t' to emulate the Tab.
bool keys__tab(V_file* const, const Config* const, const Modes* const);

// Move the cursor.
void keys__arrow_left(V_file* const, const Config* const);
void keys__arrow_right(V_file* const, const Config* const);
void keys__arrow_up(V_file* const);
void keys__arrow_down(V_file* const);

// Skip whole words.
void keys__ctrl_arrow_left(V_file* const);
void keys__ctrl_arrow_right(V_file* const);

// Scroll to the beginning/end of a file.
void keys__ctrl_arrow_up(V_file* const);
void keys__ctrl_arrow_down(V_file* const);

#endif

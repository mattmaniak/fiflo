#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when an user inserts a char.

#include "ascii.h"
#include "v_file.h"
#include "file_io.h"
#include "memory.h"

// Optionally shift a text horizontally.
bool edit__delete_char(V_file*);

// Delete a current line and decrement a lines amount indicator.
bool edit__delete_line(V_file*);

// Move a text when the cursor is shifted left and a key was pressed.
void edit__shift_text_horizonally(V_file*, const char);

/* As in the name, but supports an Y-shift with the cursor scrolled
   horizontally. */
bool edit__move_lines_forward(V_file*);

// Also shift lines up.
bool edit__move_lines_backward(V_file*);

// As in the name.
bool edit__delete_last_empty_line(V_file*);

// As in the name.
bool edit__delete_last_line(V_file*);

// When the CTRL^O is pressed, there is possibility to basically edit a fname.
void edit__filename(V_file*, const Config* const, Modes*, const char);

// Skip the Tab instantly instead of 1 column char for the first time.
void edit__skip_tab_left(V_file* const);

// As in the name.
void edit__skip_visible_chars_left(V_file* const);

// Scenario when there is a char at the beginning and the Tab at the right.
bool edit__shift_tab_from_right(V_file* const);

// Prevent removing a char and 3 tabs from e.g. "\t\t\t\t".
size_t edit__dont_delete_char_after_tab(V_file* const, const size_t,
                                        const char, const size_t);

// Some txt and the Tab(s) at the end.
bool edit__delete_char_before_tab(V_file* const, const char, const size_t);

#endif

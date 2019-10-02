#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when an user inserts a char.

#include "v_file.h"
#include "file_io.h"
#include "arrows.h"
#include "memory.h"

// Optionally shift a text horizontally.
bool edit__delete_ch(V_file_t*);

// Delete a current line and decrement a lines amount indicator.
bool edit__delete_line(V_file_t*);

// Move a text when the cursor is shifted left and a key was pressed.
void edit__shift_text_horizonally(V_file_t*, const char);

/* As in the name, but supports an Y-shift with the cursor scrolled
   horizontally. */
bool edit__move_lines_forward(V_file_t*);

// Also shift lines up.
bool edit__move_lines_backward(V_file_t*);

// As in the name.
bool edit__delete_last_empty_line(V_file_t*);

// As in the name.
bool edit__delete_last_line(V_file_t*);

// When the CTRL^O is pressed, there is possibility to basically edit a fname.
void edit__filename(V_file_t*, const Conf_t* const, Mod_t*, const char);

#endif

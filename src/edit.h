#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when You insert the char.

extern bool memory__extend_line(Buff_t*, const idx_t);
extern bool memory__shrink_current_line(Buff_t*);
extern bool memory__shrink_prev_line(Buff_t*);
extern bool memory__shrink_lines_array(Buff_t*);
extern bool memory__copy_lines_forward(Buff_t*);
extern bool memory__copy_lines_backward(Buff_t*);

extern bool file__save(Buff_t*, Conf_t*);

// Optionally shifts the text horizontally.
bool edit__delete_char(Buff_t*);

// Deletes the current line and decrements the lines index.
bool edit__delete_line(Buff_t*);

// Moves the text when the cursor is moved left and char is pressed.
void edit__shift_text_horizonally(Buff_t*, const char);

// As in the name, but supports Y shift with the cursor scrolled horizontally.
bool edit__move_lines_forward(Buff_t*);

// Also shifts lines from the bottom up.
bool edit__move_lines_backward(Buff_t*);

// Simply as in the name.
bool edit__delete_last_empty_line(Buff_t*);

// As in the name.
bool edit__delete_last_line(Buff_t*);

// When the CTRL^O is pressed, there is possibility to basically edit the fname.
void edit__filename(Buff_t*, Conf_t*, Mod_t*, const char);

#endif

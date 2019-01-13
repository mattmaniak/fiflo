#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when You insert the char.

extern bool memory_extend_line(Buff_t* Buff, idx_t line_i);
extern bool memory_shrink_act_line(Buff_t* Buff);
extern bool memory_shrink_prev_line(Buff_t* Buff);
extern bool memory_extend_lines_array(Buff_t* Buff);
extern bool memory_shrink_lines_array(Buff_t* Buff);
extern bool memory_copy_lines_forward(Buff_t* Buff);
extern bool memory_copy_lines_backward(Buff_t* Buff);

// As in the name.
bool edit_delete_last_line(Buff_t* Buff);

// Deletes the current line and decrements the lines index.
bool edit_delete_line(Buff_t* Buff);

// Moves the text when the cursor is moved left and char is pressed.
void edit_shift_text_horizonally(Buff_t* Buff, char direction);

// As in the name, but supports Y shift with the cursor scrolled horizontally.
bool edit_move_lines_forward(Buff_t* Buff);

// Simply as in the name.
bool edit_delete_last_empty_line(Buff_t* Buff);

// Also shifts lines from the bottom up.
bool edit_delete_non_last_line(Buff_t* Buff);

// Optionally shifts the text horizontally.
bool edit_delete_char(Buff_t* Buff);

#endif

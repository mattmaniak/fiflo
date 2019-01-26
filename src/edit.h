#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when You insert the char.

extern bool memory__extend_line(Buff_t* Buffer, idx_t line_i);
extern bool memory__shrink_act_line(Buff_t* Buffer);
extern bool memory__shrink_prev_line(Buff_t* Buffer);
extern bool memory__extend_lines_array(Buff_t* Buffer);
extern bool memory__shrink_lines_array(Buff_t* Buffer);
extern bool memory__copy_lines_forward(Buff_t* Buffer);
extern bool memory__copy_lines_backward(Buff_t* Buffer);

// Optionally shifts the text horizontally.
bool edit__delete_char(Buff_t* Buffer);

// Deletes the current line and decrements the lines index.
bool edit__delete_line(Buff_t* Buffer);

// Moves the text when the cursor is moved left and char is pressed.
void edit__shift_text_horizonally(Buff_t* Buffer, char direction);

// As in the name, but supports Y shift with the cursor scrolled horizontally.
bool edit__move_lines_forward(Buff_t* Buffer);

// Also shifts lines from the bottom up.
bool edit__move_lines_backward(Buff_t* Buffer);

// Simply as in the name.
bool edit__delete_last_empty_line(Buff_t* Buffer);

// As in the name.
bool edit__delete_last_line(Buff_t* Buffer);

#endif

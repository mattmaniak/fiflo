#ifndef KEYS_H
#define KEYS_H

// Currently supported ASCII characters and their actions inside.

extern bool memory_extend_line(Buff_t* Buff, idx_t line_i);
extern bool memory_shrink_act_line(Buff_t* Buff);
extern bool memory_shrink_prev_line(Buff_t* Buff);
extern bool memory_extend_lines_array(Buff_t* Buff);
extern bool memory_shrink_lines_array(Buff_t* Buff);
extern bool memory_copy_lines_forward(Buff_t* Buff);
extern bool memory_copy_lines_backward(Buff_t* Buff);

extern bool edit_delete_last_line(Buff_t* Buff);
extern bool edit_delete_line(Buff_t* Buff);
extern void edit_shift_text_horizonally(Buff_t* Buff, char direction);
extern bool edit_move_lines_forward(Buff_t* Buff);
extern bool edit_delete_last_empty_line(Buff_t* Buff);
extern bool edit_delete_non_last_line(Buff_t* Buff);
extern bool edit_delete_char(Buff_t* Buff);

extern void file_save(Buff_t* Buff);

// Knows what to do next with pressed key or combination. Based on ASCII.
bool keys_key_action(Buff_t* Buff, char key);

// Adds char when the pressed key is a printable one.
bool keys_printable_char(Buff_t* Buff, char key);

// Initialize the new line.
bool keys_linefeed(Buff_t* Buff);

// Removes a last char and optionally deletes the last line.
bool keys_backspace(Buff_t* Buff);

// These ones moves the cursor.
void keys_arrow_left(Buff_t* Buff);
void keys_arrow_right(Buff_t* Buff);
void keys_arrow_up(Buff_t* Buff);
void keys_arrow_down(Buff_t* Buff);

#endif

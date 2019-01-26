#ifndef KEYS_H
#define KEYS_H

// Currently supported characters and their actions inside.

extern bool memory__extend_line(Buff_t*, idx_t);
extern bool memory__shrink_act_line(Buff_t*);
extern bool memory__shrink_prev_line(Buff_t*);
extern bool memory__extend_lines_array(Buff_t*);
extern bool memory__shrink_lines_array(Buff_t*);
extern bool memory__copy_lines_forward(Buff_t*);
extern bool memory__copy_lines_backward(Buff_t*);

extern bool edit__delete_last_line(Buff_t*);
extern bool edit__delete_line(Buff_t*);
extern void edit__shift_text_horizonally(Buff_t*, char);
extern bool edit__move_lines_forward(Buff_t*);
extern bool edit__delete_last_empty_line(Buff_t*);
extern bool edit__delete_non_last_line(Buff_t*);
extern bool edit__delete_char(Buff_t*);

extern void file__save(Buff_t*, Conf_t*);

// Knows what to do next with pressed key or combination. Based on ASCII.
bool keys__key_action(Buff_t*, Conf_t*, char);

// Adds char when the pressed key is a printable one.
bool keys__printable_char(Buff_t*, char);

// Initialize the new line.
bool keys__linefeed(Buff_t*);

// Removes a last char and optionally deletes the last line.
bool keys__backspace(Buff_t*, Conf_t*);

// These ones moves the cursor.
void keys__arrow_left(Buff_t*, Conf_t*);
void keys__arrow_right(Buff_t*, Conf_t*);
void keys__arrow_up(Buff_t*);
void keys__arrow_down(Buff_t*);

#endif

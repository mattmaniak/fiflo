#ifndef CHARS_H
#define CHARS_H

// Currently supported characters and their actions inside.

extern bool memory__extend_line(Buff_t*, const idx_t);
extern bool memory__extend_lines_array(Buff_t*);
extern bool memory__copy_lines_forward(Buff_t*);

extern bool edit__delete_line(Buff_t*);
extern void edit__shift_text_horizonally(Buff_t*, const char);
extern bool edit__move_lines_forward(Buff_t*);
extern bool edit__delete_last_empty_line(Buff_t*);
extern bool edit__delete_char(Buff_t*);

extern bool file__save(Buff_t*, Conf_t*);

// Knows what to do next with pressed key or combination. Based on ASCII.
bool chars__parse_char(Buff_t*, Conf_t*, Mod_t*, const char);

// Adds char when the pressed key is a printable one.
bool chars__printable_char(Buff_t*, const char);

// Initialize the new line.
bool chars__linefeed(Buff_t*);

// Removes a last char and optionally deletes the last line.
bool chars__backspace(Buff_t*, Conf_t*);

// Inserts specified amount of '\t' to emulate the Tab.
bool chars__tab(Buff_t*, Conf_t*);

#endif

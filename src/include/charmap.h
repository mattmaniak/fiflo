#ifndef CHARMAP_H
#define CHARMAP_H

// Currently supported ASCII characters and their actions inside.

#include <stdnoreturn.h>

// memory.h
//extern noreturn void free_all_exit(f_mtdt* Buff, const bool status);
extern void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);
extern void safer_free(void* ptr);
extern char* extend_line_mem(f_mtdt* Buff, buff_t line_i);
extern char* shrink_act_line_mem(f_mtdt* Buff);
extern char* shrink_prev_line_mem(f_mtdt* Buff);
extern f_mtdt* extend_lines_array_mem(f_mtdt* Buff);
extern f_mtdt* shrink_lines_array_mem(f_mtdt* Buff);
extern f_mtdt* copy_lines_mem_forward(f_mtdt* Buff);
extern f_mtdt* copy_lines_mem_backward(f_mtdt* Buff);

// file.h
extern f_mtdt* save_file(f_mtdt* Buff);

// edit.h
extern f_mtdt* delete_last_line(f_mtdt* Buff);
extern f_mtdt* delete_line(f_mtdt* Buff);
extern f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction);
extern f_mtdt* move_lines_forward(f_mtdt* Buff);
extern f_mtdt* delete_last_empty_line(f_mtdt* Buff);
extern f_mtdt* delete_non_last_line(f_mtdt* Buff);
extern f_mtdt* delete_char(f_mtdt* Buff);

// Knows what to do next with pressed key or combination. Based on ASCII.
f_mtdt* key_action(f_mtdt* Buff, char key);

// Adds char when the pressed key is a printable one.
f_mtdt* printable_char(f_mtdt* Buff, char key);

// Initialize the new line.
f_mtdt* linefeed(f_mtdt* Buff);

// Removes a last char and optionally deletes the last line.
f_mtdt* backspace(f_mtdt* Buff);

static const struct
{
	f_mtdt* (*key_action)(f_mtdt* Buff, char key);
	f_mtdt* (*printable_char)(f_mtdt* Buff, char key);
	f_mtdt* (*linefeed)(f_mtdt* Buff);
	f_mtdt* (*backspace)(f_mtdt* Buff);
}
charmap =
{
	key_action,
	printable_char,
	linefeed,
	backspace
};

#endif

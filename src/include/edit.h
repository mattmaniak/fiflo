#ifndef EDIT_H
#define EDIT_H

// All the unseen magic that will happen when You insert the char.

#include <stdlib.h>
#include <stdnoreturn.h>

// edit.h
extern f_mtdt* key_action(f_mtdt* Buff, char key);

// seqmap.h
extern f_mtdt* recognize_sequence(f_mtdt* Buff, char sequence[8]);

// file.h
extern f_mtdt* save_file(f_mtdt* Buff);
extern f_mtdt* edit_fname(f_mtdt* Buff, char key);

// memory.h
//extern noreturn void free_all_exit(f_mtdt* Buff, const bool status);
extern void chk_ptr(f_mtdt* Buff, void* ptr, const char* err_msg);
extern char* extend_line_mem(f_mtdt* Buff, buff_t line_i);
extern char* shrink_act_line_mem(f_mtdt* Buff);
extern char* shrink_prev_line_mem(f_mtdt* Buff);
extern f_mtdt* extend_lines_array_mem(f_mtdt* Buff);
extern f_mtdt* shrink_lines_array_mem(f_mtdt* Buff);
extern f_mtdt* copy_lines_mem_forward(f_mtdt* Buff);
extern f_mtdt* copy_lines_mem_backward(f_mtdt* Buff);

// As in the name.
f_mtdt* delete_last_line(f_mtdt* Buff);

// Deletes the current line and decrements the lines index.
f_mtdt* delete_line(f_mtdt* Buff);

// Moves the text when the cursor is moved left and char is pressed.
f_mtdt* shift_text_horizonally(f_mtdt* Buff, char direction);

// As in the name, but supports Y shift with the cursor scrolled horizontally.
f_mtdt* move_lines_forward(f_mtdt* Buff);

// Simply as in the name.
f_mtdt* delete_last_empty_line(f_mtdt* Buff);

// Also shifts lines from the bottom up.
f_mtdt* delete_non_last_line(f_mtdt* Buff);

// Optionally shifts the text horizontally.
f_mtdt* delete_char(f_mtdt* Buff);

static const struct
{
	f_mtdt* (*delete_last_line)(f_mtdt* Buff);
	f_mtdt* (*delete_line)(f_mtdt* Buff);
	f_mtdt* (*shift_text_horizonally)(f_mtdt* Buff, char direction);
	f_mtdt* (*move_lines_forward)(f_mtdt* Buff);
	f_mtdt* (*delete_last_empty_line)(f_mtdt* Buff);
	f_mtdt* (*delete_non_last_line)(f_mtdt* Buff);
	f_mtdt* (*delete_char)(f_mtdt* Buff);
}
edit =
{
	delete_last_line,
	delete_line,
	shift_text_horizonally,
	move_lines_forward,
	delete_last_empty_line,
	delete_non_last_line,
	delete_char
};

#endif
